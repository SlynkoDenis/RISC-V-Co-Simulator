#include "co_simulator.h"
#include <iterator>


namespace {
template<typename InputIterator1, typename InputIterator2>
bool RangeEqual(InputIterator1 first1, InputIterator1 last1,
                InputIterator2 first2, InputIterator2 last2) {
    int counter = 0;
    while (first1 != last1 && first2 != last2) {
        ++counter;
        if (*first1 != *first2) {
            return false;
        }
        ++first1;
        ++first2;
    }
    return (first1 == last1) && (first2 == last2);
}
}   // end anonymous namespace

namespace cosimulator {
bool RV32ICoSimulator::SetupBeforeTest() {
    functional_model_.MakeTracesReadable(readable_traces_);
    pipeline_model_.MakeTracesReadable(readable_traces_);
    return SetupDirectory(TracesDir)
           && SetupFiles(FunctionalTraces)
           && SetupFiles(PipelineTraces);
}

void RV32ICoSimulator::TeardownAfterTest() {
    functional_model_.DisableTraces();
    pipeline_model_.DisableTraces();
    if (should_delete_traces_) {
        RemoveDirectory(TracesDir);
    }
}

int RV32ICoSimulator::RunTest(const char *path) {
    functional_model_.EnableTraces(FunctionalTraces, TraceFilesModes);
    auto ret_code = functional_model_.RunProgram(path);
    if (!IsReturnCodeValid(ret_code)) {
        std::cerr << "Functional Model failed with code " << static_cast<int>(ret_code);
        std::cerr << ' ' << ret_code << std::endl;
        return static_cast<int>(RV32ICoSimResult::FUNCTIONAL_FAILED);
    }

    pipeline_model_.EnableTraces(PipelineTraces, TraceFilesModes);
    ret_code = pipeline_model_.RunProgram(path);
    if (!IsReturnCodeValid(ret_code)) {
        std::cerr << "Pipeline Model failed with code " << static_cast<int>(ret_code);
        std::cerr << ' ' << ret_code << std::endl;
        return static_cast<int>(RV32ICoSimResult::PIPELINE_FAILED);
    }

    return CompareTraces();
}

int RV32ICoSimulator::CompareTraces() const {
    // TODO: add other checks?
    return CompareRegFileTraces() | CompareMMUTraces() | CompareExecutionPathTraces();
}

int RV32ICoSimulator::CompareRegFileTraces() const {
    auto error_code = static_cast<int>(RV32ICoSimResult::REGFILE_MISMATCH);

    std::ifstream funct_regfile(FunctionalTraces[1]);
    if (!funct_regfile.is_open()) {
        WARNING("failed to open trace file " + std::string(FunctionalTraces[1]));
        return error_code;
    }
    std::ifstream pipe_regfile(PipelineTraces[1]);
    if (!pipe_regfile.is_open()) {
        WARNING("failed to open trace file " + std::string(PipelineTraces[1]));
        return error_code;
    }

    // TBD
    return 0;
}

int RV32ICoSimulator::CompareMMUTraces() const {
    auto error_code = static_cast<int>(RV32ICoSimResult::MMU_MISMATCH);

    std::ifstream funct_regfile(FunctionalTraces[0]);
    if (!funct_regfile.is_open()) {
        WARNING("failed to open trace file " + std::string(FunctionalTraces[0]));
        return error_code;
    }
    std::ifstream pipe_regfile(PipelineTraces[0]);
    if (!pipe_regfile.is_open()) {
        WARNING("failed to open trace file " + std::string(PipelineTraces[0]));
        return error_code;
    }

    // TBD
    return 0;
}

int RV32ICoSimulator::CompareExecutionPathTraces() const {
    auto error_code = static_cast<int>(RV32ICoSimResult::EXEC_PATH_MISMATCH);

    std::basic_ifstream<char32_t> funct_ifs(FunctionalTraces[2], std::ifstream::binary);
    if (!funct_ifs.is_open()) {
        WARNING("failed to open trace file " + std::string(FunctionalTraces[2]));
        return error_code;
    }
    std::basic_ifstream<char32_t> pipe_ifs(PipelineTraces[2], std::ifstream::binary);
    if (!pipe_ifs.is_open()) {
        WARNING("failed to open trace file " + std::string(PipelineTraces[2]));
        return error_code;
    }

    std::istreambuf_iterator<char32_t> funct_begin(funct_ifs);
    std::istreambuf_iterator<char32_t> pipe_begin(pipe_ifs);
    std::istreambuf_iterator<char32_t> end;
    if (!RangeEqual(funct_begin, end, pipe_begin, end)) {
        return error_code;
    }
    return 0;
}

bool RV32ICoSimulator::IsReturnCodeValid(runtime::ReturnCodes code) {
    return (code == runtime::ERROR_NONE || code == runtime::ECALL || code == runtime::EBREAK);
}

bool RV32ICoSimulator::SetupDirectory(const char *path) {
    if (!utils::CreateDirectoryIfNotExists(path)) {
        WARNING("failed to mkdir " + std::string(path));
        return false;
    }
    return true;
}

bool RV32ICoSimulator::RemoveDirectory(const char *path) {
    if (!utils::RemoveDirectory(path)) {
        WARNING("failed to remove directory " + std::string(path));
        return false;
    }
    return true;
}


std::string CodeStringRepresentation(int code) {
    switch (code) {
    default:
        break;
    case -1:
        return "(SETUP_FAIL)";
    case 0:
        return "(ERROR_NONE)";
    case 1:
        return "(FUNCTIONAL_FAILED)";
    case 2:
        return "(PIPELINE_FAILED)";
    }

    bool need_comma = false;
    std::ostringstream oss;
    oss << '(';
    if (code & 0b100) {
        oss << "REGFILE_MISMATCH";
        need_comma = true;
    }
    if (code & 0b1000) {
        if (need_comma) {
            oss << ", ";
        }
        oss << "MMU_MISMATCH";
        need_comma = true;
    }
    if (code & 0b10000) {
        if (need_comma) {
            oss << ", ";
        }
        oss << "EXEC_PATH_MISMATCH";
    }
    oss << ')';
    return oss.str();
}
}   // end namespace cosimulator

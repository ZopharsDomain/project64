#pragma once
#include <stdint.h>

extern uint16_t Set_AudioHle, Set_GraphicsHle, Set_AllocatedRdramSize, Set_DirectoryLog;
extern bool GraphicsHle, AudioHle, ConditionalMove, SyncCPU, RspMultiThreaded;
extern bool DebuggingEnabled, Profiling, IndvidualBlock, ShowErrors, BreakOnStart, LogRDP, LogAsmCode;

enum class RSPCpuMethod
{
    Interpreter = 0,
#if defined(__i386__) || defined(_M_IX86)
    Recompiler = 1,
#endif
#if defined(__amd64__) || defined(_M_X64)
    RecompilerTasks = 2,
#endif
    HighLevelEmulation = 3,
};

class CRSPSettings
{
public:
    CRSPSettings();
    virtual ~CRSPSettings();

    inline static RSPCpuMethod CPUMethod(void)
    {
        return m_CPUMethod;
    }
    inline static bool RomOpen(void)
    {
        return m_RomOpen;
    }

    static void EnableDebugging(bool Enabled);
    static void InitializeRspSetting(void);
    static void RefreshSettings(void);

    static void SetRomOpen(bool Opened);

private:
    CRSPSettings(const CRSPSettings &);
    CRSPSettings & operator=(const CRSPSettings &);

    static bool m_DebuggingEnabled;
    static bool m_RomOpen;
    static RSPCpuMethod m_CPUMethod;
};

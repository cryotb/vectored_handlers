#include "inc/include.h"

/*
*   Locates `LdrpVectorHandlerList` based on a simple algorithm
*/
uintptr_t locate_veh_list()
{
    auto handle_k32 = GetModuleHandleA("kernel32.dll");

    uintptr_t rs = 0;
    uintptr_t fn_add_veh = (uintptr_t)GetProcAddress(handle_k32, "AddVectoredExceptionHandler");
    uintptr_t start = resolve_oinsn(fn_add_veh + 3, 1, 5);
    uintptr_t curr = start;

    std::vector<ZydisDecodedInstruction> disasm;
    if (zy::DisasmBuffer(reinterpret_cast<uint8_t*>(start), 512, disasm))
    {
        for (const auto& insn : disasm)
        {
            if (insn.mnemonic == ZYDIS_MNEMONIC_AND)
            {
                if (insn.operand_count == 3 && insn.operands[0].type == ZYDIS_OPERAND_TYPE_REGISTER)
                {
                    if (insn.operands[1].type == ZYDIS_OPERAND_TYPE_IMMEDIATE &&
                        insn.operands[1].imm.value.u == 0x3F)
                    {
                        curr += insn.length;
                        rs = resolve_oinsn(curr, 3, 7);
                        break;
                    }
                }
            }

            curr += insn.length;
        }
    }

    return rs;
}

vectored_handlers::vectored_handlers(uint8_t mode)
{
    this->_mode = mode;

    _list = get_ve_handler_list(_mode);
    _lock = reinterpret_cast<PSRWLOCK>(get_ve_lock(_mode));
}

uintptr_t vectored_handlers::get_ve_handler_list(int mode)
{
    static uintptr_t veh_list = 0;
    if (!veh_list) veh_list = locate_veh_list();

    return reinterpret_cast<uintptr_t*>(veh_list)[3 * mode + 1];
}

uintptr_t vectored_handlers::get_ve_lock(int mode)
{
    static uintptr_t veh_list = 0;
    if (!veh_list) veh_list = locate_veh_list();

    return reinterpret_cast<uintptr_t*>(veh_list)[3 * mode];
}

struct vh_record_internal
{
    void* unk_ptr_1;        // 0x00
    void* unk_ptr_2;        // 0x08
    char  pad_10[0x10];     // 0x10
    void* handler;          // 0x20
};

std::vector<vh_record> vectored_handlers::list()
{
    std::vector<vh_record> rs;

    auto list_end = reinterpret_cast<void**>(_list);

    for (auto record = reinterpret_cast<void**>(*list_end); ;
        record = reinterpret_cast<void**>(*record))
    {
        if (record == list_end)
        {
            break;
        }

        auto precord = reinterpret_cast<vh_record_internal*>(record);

        auto decrypted_handler = DecodePointer(precord->handler);

        vh_record rec;
        memset(&rec, 0, sizeof(rec));
        rec.handle = record;
        rec.handler = decrypted_handler;
        rec.phandler = &precord->handler;
        rs.push_back(rec);
    }

    return rs;
}

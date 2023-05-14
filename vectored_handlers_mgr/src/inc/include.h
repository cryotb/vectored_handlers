#pragma once

#include <Zydis/Zydis.h>

#include <vector>
#include <Windows.h>

#define resolve_oinsn(addr, instr_off, instr_len) ((addr + *reinterpret_cast<int32_t *>(addr + instr_off)) + instr_len)

#include "wzydis.h"
#include "vectored_handlers.h"

#include "tests.h"

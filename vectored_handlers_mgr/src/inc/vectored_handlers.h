#pragma once

/*
*   Little helper class for automatically managing SRW locks.
*/
class srw_lock_guard
{
public:
    srw_lock_guard(PSRWLOCK plock)
    {
        _plock = plock;
        if (_plock) AcquireSRWLockExclusive(_plock);
    }

    ~srw_lock_guard()
    {
        if (_plock) ReleaseSRWLockExclusive(_plock);
    }
private:
    PSRWLOCK _plock;
};

#define VH_MODE_EXCEPTION 0
#define VH_MODE_CONTINUE 1

/*
*   The layout of a single vectored handler.
*/
struct vh_record
{
    void* handle;
    void* handler;
    void** phandler;
};

/*
*   Helper class to interact with VHs.
*/
class vectored_handlers
{
public:
    bool valid() { return _list != 0 && _lock != nullptr; }
public:
    vectored_handlers() = delete;
    vectored_handlers(uint8_t mode);
public:
    uintptr_t locate();
    auto plock() { return _lock; }

    std::vector<vh_record> list();
    bool empty() { return list().empty(); }

    const auto find_by_handle(void* val)
    {
        auto rs = vh_record{ 0, nullptr };
        for (const auto& vh : list())
        {
            if (vh.handle == val)
                rs = vh;
        }

        return vh_record{ 0, nullptr };
    }

    bool hijack(void* handle, void* nhandler)
    {
        for (const auto& vh : list())
        {
            if (vh.handle == handle)
            {
                *vh.phandler = EncodePointer(nhandler);
                return true;
            }
        }

        return false;
    }
private:
    uintptr_t get_ve_handler_list(int mode);
    uintptr_t get_ve_lock(int mode);
private:
    uint8_t _mode;
    uintptr_t _list;
    PSRWLOCK _lock;
};
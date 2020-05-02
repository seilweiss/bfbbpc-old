#include "iFile.h"

#include "xFile.h"

#include <string.h>
#include <windows.h>

struct AsyncRequest
{
    bool inUse;
    int id;
    void *buf;
    unsigned int size;
    IFILE_READSECTOR_STATUS status;
    iFileReadCallBack callback;
    tag_xFile *file;
};

static char sBasePath[256];
static AsyncRequest requests[32];

void iFileInit()
{
    GetCurrentDirectory(256, sBasePath);
}

unsigned int iFileOpen(const char *name, int flags, tag_xFile *file)
{
    tag_iFile *ps = &file->ps;
    DWORD access = 0;

    if (flags & 0x4)
    {
        strcpy(ps->path, name);
    }
    else
    {
        iFileFullPath(name, ps->path);
    }

    if (flags & 0x1)
    {
        access |= GENERIC_READ;
    }

    if (flags & 0x2)
    {
        access |= GENERIC_WRITE;
    }

    ps->handle = CreateFile(ps->path, access, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                            FILE_FLAG_OVERLAPPED, NULL);

    if (ps->handle == INVALID_HANDLE_VALUE)
    {
        return 1;
    }

    ps->flags = IFILE_ISOPEN;
    ps->length = GetFileSize(ps->handle, NULL);

    iFileSeek(file, 0, IFILE_SEEK_SET);

    return 0;
}

int iFileSeek(tag_xFile *file, int offset, int whence)
{
    tag_iFile *ps = &file->ps;
    int new_pos;

    switch (whence)
    {
    case IFILE_SEEK_SET:
        new_pos = offset;
        break;

    case IFILE_SEEK_CUR:
        new_pos = offset + ps->offset;
        break;

    case IFILE_SEEK_END:
        new_pos = ps->length - ps->offset;
        break;

    default:
        new_pos = offset;
    }

    ps->offset = new_pos;

    return ps->offset;
}

unsigned int iFileRead(tag_xFile *file, void *buf, unsigned int size)
{
    tag_iFile *ps = &file->ps;
    DWORD num = -1;
    BOOL status;

    ps->overlapped.Internal = 0;
    ps->overlapped.InternalHigh = 0;
    ps->overlapped.Offset = ps->offset;
    ps->overlapped.OffsetHigh = 0;
    ps->overlapped.hEvent = 0;

    status = ReadFile(ps->handle, buf, size, NULL, &ps->overlapped);

    if (status || GetLastError() == ERROR_IO_PENDING)
    {
        while (!GetOverlappedResult(ps->handle, &ps->overlapped, &num, TRUE))
        {
            if (GetLastError() != ERROR_IO_PENDING)
            {
                num = -1;
                break;
            }
        }
    }
    else
    {
        num = -1;
    }

    return num;
}

static void __stdcall async_cb(DWORD dwErrorCode, DWORD dwNumberOfBytesTransferred,
                               LPOVERLAPPED lpOverlapped)
{
    AsyncRequest *request = &requests[(int)lpOverlapped->hEvent];

    request->status = IFILE_RDSTAT_DONE;
}

static DWORD WINAPI iFileReadAsyncThread(LPVOID lpParam)
{
    AsyncRequest *request = (AsyncRequest *)lpParam;
    tag_iFile *ps = &request->file->ps;
    DWORD bytesRead = 0;

    if (!ReadFileEx(ps->handle, request->buf, request->size, &ps->overlapped, async_cb))
    {
        request->inUse = false;
        return 1;
    }

    request->status = IFILE_RDSTAT_INPROG;

    while (!GetOverlappedResultEx(ps->handle, &ps->overlapped, &bytesRead, INFINITE, TRUE))
    {
        if (GetLastError() != WAIT_IO_COMPLETION)
        {
            request->inUse = false;
            return 1;
        }
    }

    return 0;
}

int iFileReadAsync(tag_xFile *file, void *buf, unsigned int aSize,
                   iFileReadCallBack callback, int priority)
{
    tag_iFile *ps = &file->ps;
    int i;
    int id = -1;
    AsyncRequest *request;
    HANDLE thread;

    for (i = 0; i < 32; i++)
    {
        request = &requests[i];

        if (!request->inUse || request->status == IFILE_RDSTAT_DONE)
        {
            id = i;

            request->inUse = true;
            request->id = id;
            request->buf = buf;
            request->size = aSize;
            request->status = IFILE_RDSTAT_QUEUED;
            request->callback = callback;
            request->file = file;

            ps->asynckey = id;

            ps->overlapped.Internal = 0;
            ps->overlapped.InternalHigh = 0;
            ps->overlapped.Offset = ps->offset;
            ps->overlapped.OffsetHigh = 0;
            ps->overlapped.hEvent = (HANDLE)id;

            thread = CreateThread(NULL, 0, iFileReadAsyncThread, request, 0, NULL);

            if (!thread)
            {
                request->inUse = false;
                id = -1;
            }

            break;
        }
    }

    return id;
}

IFILE_READSECTOR_STATUS iFileReadAsyncStatus(int key, int *amtToFar)
{
    AsyncRequest *request = &requests[key];

    if (request->id != key)
    {
        return IFILE_RDSTAT_EXPIRED;
    }

    if (amtToFar)
    {
        *amtToFar = request->size;
    }

    return request->status;
}

void iFileAsyncService()
{
    return;
}

unsigned int iFileClose(tag_xFile *file)
{
    if (!CloseHandle(file->ps.handle))
    {
        return 1;
    }

    file->ps.flags = 0;
    return 0;
}

unsigned int iFileGetSize(tag_xFile *file)
{
    return file->ps.length;
}

void iFileFullPath(const char *relname, char *fullname)
{
    strcpy(fullname, sBasePath);
    strcat(fullname, "\\");
    strcat(fullname, relname);
}

void iFileSetPath(char *path)
{
    strcpy(sBasePath, path);
}

unsigned int iFileFind(const char *name, tag_xFile *file)
{
    return iFileOpen(name, 0, file);
}

void iFileGetInfo(tag_xFile *file, unsigned int *offset, unsigned int *length)
{
    if (offset)
    {
        *offset = file->ps.offset;
    }

    if (length)
    {
        *length = file->ps.length;
    }
}
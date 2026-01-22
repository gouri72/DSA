@echo off
echo Building Clinic Server (Multifile)...
gcc backend/server.c backend/clinic.c backend/avl.c backend/event_list.c -o server -lws2_32
if %errorlevel% neq 0 (
    echo Compilation Failed!
    pause
    exit /b
)
echo Build Success!
echo Starting Server...
server.exe

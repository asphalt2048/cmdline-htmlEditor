@echo off
setlocal

set TEST_DIR=build\test

echo Running all test executables in %TEST_DIR% ...
echo.

REM 遍历所有 .exe 测试文件
for %%F in (%TEST_DIR%\*.exe) do (
    echo -------------------------
    echo Running %%~nF
    echo -------------------------
    "%%F"
    echo.
)

echo All tests completed.
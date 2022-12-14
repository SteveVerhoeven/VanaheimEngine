pushd ..\
call Libs\Premake\premake5.exe vs2022

start VanaheimEngine.sln
popd

PAUSE
set Toolset=v110
set LibsFolder=..\libs\%Toolset%


rmdir protobuf\vsprojects\debug /S /Q
rmdir protobuf\vsprojects\release /S /Q
rmdir %LibsFolder% /S /Q

REM %%%%%%%%% Protobuf (debug and release, Win32) %%%%%%%%%%%
msbuild protobuf\vsprojects\libprotobuf.sln /p:Configuration=Debug
mkdir %LibsFolder%\win32\debug\libprotobuf\
copy protobuf\vsprojects\debug\libprotobuf.lib %LibsFolder%\win32\debug\libprotobuf\
copy protobuf\vsprojects\debug\*.pdb %LibsFolder%\win32\debug\libprotobuf\

msbuild protobuf\vsprojects\libprotoc.sln /p:Configuration=Debug
mkdir %LibsFolder%\win32\debug\libprotoc\
copy protobuf\vsprojects\debug\libprotoc.lib %LibsFolder%\win32\debug\libprotoc\
copy protobuf\vsprojects\debug\*.pdb %LibsFolder%\win32\debug\libprotoc\

msbuild protobuf\vsprojects\libprotobuf.sln /p:Configuration=Release
mkdir %LibsFolder%\win32\release\libprotobuf\
copy protobuf\vsprojects\release\libprotobuf.lib %LibsFolder%\win32\release\libprotobuf\
copy protobuf\vsprojects\release\*.pdb %LibsFolder%\win32\release\libprotobuf\

msbuild protobuf\vsprojects\libprotoc.sln /p:Configuration=Release
mkdir %LibsFolder%\win32\release\libprotoc\
copy protobuf\vsprojects\release\libprotoc.lib %LibsFolder%\win32\release\libprotoc\
copy protobuf\vsprojects\release\*.pdb %LibsFolder%\win32\release\libprotoc\

REM %%%%%%%%% Glog (debug and release, Win32) %%%%%%%%%%%

rmdir glog\debug /S /Q
rmdir glog\release /S /Q

msbuild glog\libglog.sln /p:Configuration=Debug
msbuild glog\libglog.sln /p:Configuration=Release
copy glog\debug\* %LibsFolder%\win32\debug\*
copy glog\release\* %LibsFolder%\win32\release\*

REM %%%%%%%%% rpcz (debug and release, Win32) %%%%%%%%%%%
rmdir rpcz\vsprojects\Intermediate /S /Q
rmdir rpcz\vsprojects\Win32 /S /Q

msbuild rpcz\vsprojects\librpcz.sln /p:Configuration=Debug
mkdir %LibsFolder%\win32\debug\librpcz\
copy rpcz\vsprojects\win32\debug\librpcz.lib %LibsFolder%\win32\debug\librpcz\
copy rpcz\vsprojects\Intermediate\win32_%Toolset%\Debug\librpcz\*.pdb %LibsFolder%\win32\debug\librpcz\

msbuild rpcz\vsprojects\librpcz.sln /p:Configuration=Release
mkdir %LibsFolder%\win32\release\librpcz\
copy rpcz\vsprojects\win32\release\librpcz.lib %LibsFolder%\win32\release\librpcz\
copy rpcz\vsprojects\Intermediate\win32_%Toolset%\Release\librpcz\*.pdb %LibsFolder%\win32\release\librpcz\

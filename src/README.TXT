Building with Visual Studio 2010
1. Download and unpack boost 1.55 (http://sourceforge.net/projects/boost/files/)
   Set environmental variable BOOST_ROOT to the root of our boost installation. To do so you may use cmd.exe:
 	setx BOOST_ROOT "C:\Program Files\boost\boost_1_55_0"

2. Download and unpack protobuf 2.5.0 (https://protobuf.googlecode.com/files/protobuf-2.5.0.zip).   
   Set environmental variable PROTOBUF_ROOT to the root protocol buffers library.
	setx PROTOBUF_ROOT "C:\Program Files\protobuf\src"

3. Open artm_vs2010.sln or artm_vs2012.sln (depending on your visual studio version)
   Ensure to first close and reopen all existing instances of visuals studio!

4. Build in either win32-debug or win32-release configuration.

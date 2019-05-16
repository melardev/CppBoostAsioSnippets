# Introduction
This repository contains Boost Cpp Asio snippets.

# Getting Started
For Cpp nowadas there are no excuses to setup your environment in a painless manner, this is why<br>
I recommend using vcpkg, which is what I use, with `vcpkg install boost:x64-windows-static` it downloaded and built<br>
boost libraries. Then I open the vcproject file and add the following inside the Globals section:
```xml
<PropertyGroup Label="Globals"> <-- Find this tag, then in that tag add the two VcpkgTriplet tags-->
............
<VcpkgTriplet Condition="'$(Platform)'=='Win32'">x86-windows-static</VcpkgTriplet>
<VcpkgTriplet Condition="'$(Platform)'=='x64'">x64-windows-static</VcpkgTriplet>
..........
```

This is how I set up my environment, I strongly encourage you to do the same, take a time reading about [vcpkg](https://github.com/microsoft/vcpkg)<br>
and enjoy modern Cpp made easy.

# Social Media
- [Youtube](https://youtube.com/Melardev)
- [Twitter](https://twitter.com/@melardev)
- [Instagram](https://instagram.com/melar_dev)

# TODO
- Port to CMake
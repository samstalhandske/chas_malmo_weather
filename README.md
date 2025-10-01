# Chas Academy Malmö Weatherclient

This project is a work in progress weatherclient that is currently being written in **C90**

## Prerequisites to build
### Linux
*   **gcc**
*   **libcurl**
*   **make**
### Windows
*   **premake**
*   **vcpkg**
*   **Visual Studio 2022 or higher**

## Building
### Ubuntu
1. `sudo apt install make gcc libcurl4-openssl-dev`
2. `git clone https://github.com/samstalhandske/chas_malmo_weather.git`
3. `cd chas_malmo_weather`
4. `make`
### Fedora
1. `sudo dnf install make gcc libcurl-devel`
2. `git clone https://github.com/samstalhandske/chas_malmo_weather.git`
3. `cd chas_malmo_weather`
4. `make`
### Windows (Experimental)
1. `.\vcpkg install curl:windows-x86`
2. `git clone https://github.com/samstalhandske/chas_malmo_weather.git`
4. `cd chas_malmo_weather`
4. `premake5 vs2022` 
5. `Now you should have a .sln project file, click on it and Build -> Build Solution`
6. `Now you are good to go!`

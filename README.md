# Chas Academy Malmö Weatherclient

This project is a work in progress weatherclient that is currently being written in **C90**

## What the WeatherClient does
`This project is a console-based weather application written in C. It allows users to interactively select cities, fetch weather reports, and manage a local cache of city data. The application features an interactive command-line interface, supports adding, removing, and editing city entries, and retrieves weather data (from an API via HTTP requests). City and weather data are serialized to and loaded from JSON files.`

`Key Features:`

* `Interactive console UI for selecting cities and viewing weather`
* `Local cache of cities managed via JSON files`
* `Modular structure: distinct components for city management, weather retrieval, HTTP communication, and UI`
* `Uses libcurl for HTTP requests and cJSON for JSON serialization/deserialization`
* `Custom utilities for string manipulation, input handling, and file operations`

`Technical Stack:`

* `Language: C, with Makefile for build automation`
* `Libraries: cJSON (for JSON handling), libcurl (for HTTP requests) - working on support for Jansson`
* `Code is organized for maintainability and extensibility`

`Typical Usage Flow:`

1. `User starts the program and is greeted with a menu.`
2. `User selects a city or manages the city list (add, edit, remove).`
3. `Weather data is fetched and displayed for the selected city.`
4. `Data is cached locally to speed up subsequent access and reduce API calls.`

## Demo
```bash
$ ./chas_malmo_weather
<===================# Welcome to the Weather App! #===================>

Malmö
Stockholm
Gothenburg

Options(O/0) for options, Exit(E/Q) to exit.
Select a city: Malmö

Current weather for Malmö:
Temperature: 18°C
Condition: Sunny
Humidity: 60%
```

## Prerequisites to build

*   **gcc**
*   **libcurl**
*   **cJSON** (included in `src/mcore/cJSON`)
*   **make**

## Building
`Designed for Ubuntu and Fedora. Working on extended support!`
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

## Usage
1. **Start the app:**  
   `./chas_malmo_weather`
2. **Select a city** from the list or add your own.
3. **View weather reports**, manage your city list, or exit.

## Project Structure
```txt
src/
├── main.c                # App entry point
├── ui/ClientUI.c         # Console UI logic
├── city/City.c           # City management
├── weather/Weather.c     # Weather fetching & display
├── mcore/http/http.c     # HTTP requests (libcurl)
├── mcore/json/cJSON/     # JSON parsing library
└── ...                   # Utilities and helpers
```

## How to Contribute
1. `Clone repo and create a new branch: $ git checkout https://github.com/samstalhandske/chas_malmo_weather -b new_branch_not_yet_named.`
2. `Make changes and test.`
3. `Submit pull request with clear and descriptive change notes`

## Donations
`This is a project from the Malmo-branch of CHAS Academy and we are all students working on this project and as of now this is just a school project, so we are not accepting any donations. But be on the lookout for upcoming projects`

## Acknowledgments
- [cJSON](https://github.com/DaveGamble/cJSON) for JSON parsing
- [libcurl](https://curl.se/libcurl/) for HTTP requests

## Maintainers
- [samstalhandske](https://github.com/samstalhandske)
- [pontusrosenquist](https://github.com/pontusrosenquistgmailcom)
- [Emilio](https://github.com/AlCapone1234) / [Emilios other account](https://github.com/samstalhandske/chas_malmo_weather/commits?author=pooppoop)
- [lukas.stade96@gmail.com](https://github.com/HoffaQt)
- [Pär Lundh](https://github.com/lundhpargmailcom)
- [Henrik Westerlund](https://github.com/Henrik-Westerlund)
- [Isa Shipshani](https://github.com/isashiphotmailcom)
- [Jonathan Alestam](https://github.com/JonathanAlestam)

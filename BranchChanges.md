# Changelog (partial)
- cachedcity json data saves **latitude and longitude numbers (double)** instead of char string. *Delete cachedcity folder in case of parse errors.*
- Many functions related to linked list moved from `City.c` to `LinkedListCity.c`
- `City_AddCityToLinkedList()` now handles saving City as json data in cache.
- `City_SaveJsonToFile()` now takes a `City*` as argument instead of multiple variables.
- `Weather_GetReport()` now takes a `City*` as argument instead of multiple variables.
- Implemented pooppoop's improved `City_EditCity` function.
- renamed UserInteractionFunctions to `ClientUI.c/.h`
- `WeatherReport` is now stored as `WeatherData` in `City*` in `LinkedListCity` nodes. function to free stored reports implemented in end of program function.
- A new OpenMeteo API url has been implemented - precipitation and other new data could be implemented.
    * default time to GMT +0
    * default windspeed in m/s
- New functions that constructs jsonfile names.
    * `char* ConstructCityCacheFilePath(City* _City);`
    * `char* ConstructReportCacheFilePath(City* _City);`
- Some code documentation added.
- Some memory improvements.
- Many other smaller (or larger) changes that the author does not remember.
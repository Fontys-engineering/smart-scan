# SmartScan Software Repository

## Folder Structure:
1. Root
    1. SmartScanCLI - Command Line User Interface implementation of the library
    2. SmartScanService - The SmartScan library 

## Getting started:
* Download a git client (i.e. [Github Desktop](https://desktop.github.com/) )
* Clone this repository to a desired location
* Open the .sln file from SmartScanCLI with **Visual Studio 2019**
* Switch to Debug/x64 (if necessary)
* (if necessary) Go to project>SmartScanCLI Properties
* * In C++ add the path to the SmartScanService.h file, and the path to ATC3DG.h (inside the NDI folder) to "Additional Include directories"
* * In Linker>General add the path to the SmartScanService.lib file (SmartScanService/x64/Debug/) and the path to ATC3DG.lib (SmartScanService/NDI/) to "Additional Library Directories". if SmartScanService.lib is missing, build the SmartScanService project first
* * In Linker>Input add the complete path (including the lib name and extension) to both SmartScanService.lib and ATC3DG.lib to "Additional Deppendencies"
* Pick between mock data mode or real data mode by changin the variable in SmartScanCLI.h
* Run (F5). It will fail due to missing ATC3DG64.DLL.
* Add the "ATC3DG64.DLL" file from SmartScanService/NDI to the same folder as the generated SmartScanCLI.exe (x64/Debug/)
* Run (F5).

## Development workflow:
### Git
**NEVER WORK ON THE MAIN OR DEVELOP BRANCHES DIRECTLY**
1. PULL the latest changes from upstream (i.e. in Github desktop: Repository>pull)
2. Create a new branch for every important feature or fix you add (i.e. in Github desktop go to branch>new branch or ctrl+shift+n). name the branch accordingly (e.g. _feature/implement-filtering_ or _bugfix/CSV-memory-leak_)
3. If a branch already exists (someone else created it), switch to it instead. (in github desktop use the "currrent branch" dropdown and pick your branch)
4. Write/add your new code or make your changes.
5. Commit your changes to your branch. Make sure to add a helpful message (summary) about what you have done (e.g. _Added public memmbers to the SmartScanService class_)
6. Push changes upstream (i.e. in Github desktop: Repository>push)
7. Repeat steps 4 5 and 6 until the feature or fix is done.
8. Implement your changes into the develop branch by doing a *pull request*. (in github desktop: Branch> create pull request or ctrl+R). Make sure to add the repositiory maintainer (Eduard) and at least one of your team members as a reviewer. Once all reviewers have approved the pull request, the maintainer (Eduard) will merge the changes into the develop branch.

### C++
#### SmartScanService
1. Make desired changes
2. Build for x64/Debug
3. Test by building and running SmartScanCLI for x64/Debug

#### SmartScanCLI
1. Make changes 
2. Make sure there is a SmartScanService.lib already compiled, otherwise build SmartScanService first
3. Build for x64/Debug
4. Run the generated .exe (make sure you have ATC3DG64.DLL in the same folder)
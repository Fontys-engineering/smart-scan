#SmartScan Software Repository

##Folder Structure:
1. Root
    1. Matlab = Matlab code used for the filtering
    2. SmartScanService - The main c++ project

##Getting started:
* Download a git client (i.e. [Github Desktop](https://desktop.github.com/) )
* Clone this repository to a desired location
* Open the .sln file with Visual Studio 2019
* Run (F5)


##Development workflow:
###Git
**NEVER WORK ON THE MAIN OR DEVELOP BRANCHES DIRECTLY**
1. PULL the latest changes from upstream (i.e. in Github desktop: Repository>pull)
2. Create a new branch for every important feature or fix you add (i.e. in Github desktop go to branch>new branch or ctrl+shift+n). name the branch accordingly (e.g. _feature/implement-filtering_ or _bugfix/CSV-memory-leak_)
3. If a branch already exists (someone else created it), switch to it instead. (in github desktop use the "currrent branch" dropdown and pick your branch)
4. Write/add your new code or make your changes.
5. Commit your changes to your branch. Make sure to add a helpful message (summary) about what you have done (e.g. _Added public memmbers to the SmartScanService class_)
6. Push changes upstream (i.e. in Github desktop: Repository>push)
7. Repeat steps 4 5 and 6 until the feature or fix is done.
8. Implement your changes into the develop branch by doing a *pull request*. (in github desktop: Branch> create pull request or ctrl+R). Make sure to add the repositiory maintainer (Eduard) and at least one of your team members as a reviewer. Once all reviewers have approved the pull request, the maintainer (Eduard) will merge the changes into the develop branch.

###C++
To be added.
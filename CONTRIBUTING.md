# Version number
Versioning standard to use:
```
<major>.<minor>.<revision>
```

# Making changes and branching
This repo follows the branching model explained in [this](https://nvie.com/posts/a-successful-git-branching-model/
) fantastic post. 

## Main branches
The repo has 2 main branches, `master` and `develop`.

We consider `master` to be the main branch where the source code of HEAD always reflects a production-ready state.

We consider `develop` to be the main branch where the source code of HEAD always reflects a state with the latest delivered development changes for the next release.

## Supporting branches
Besides main branches the repo has an extra set of branches that support main ones. This branches are `feature`, `release` and `hotfix` branches.

### Feature branches
Feature branches contain a new set of features. Once all changes are implemented and tested in a feature branch, the way to go is a pull request to the branch `develop`. The naming style for this branches is `snake_case`. Ex:
```
feature/kickass_new_feature
```

### Bug branches
Bug branches contain fixes for bugs that may appear in feature branches changes that have been merged into `develop`, but not to `release`or `master` branches. The naming style for this branches is `snake_case`. Ex:
```
bug/fix_stupid_bug
```

### Release branches
Release branches support preparation of a new production release. They are brached from `develop` when the code is ready to go to production, and basically they contain small bug fixes and changes in meta-data needed for release (*i.e* change version number). The naming style for this branches is `snake_case`. Ex:
```
release/v1.0
```

### Hotfix branches
Hotfix branches are very much like release branches beacause they also prepare for a new production release, albeit unplanned. They arise from the necessity to act immediately upon an undesired state of a live production version. The naming style for this branches is `snake_case`. Ex:
```
hotfix/v1.0.1
```

# C++ Coding Style Guidelines
## Naming Style
Functions are named using [PascalCase](https://techterms.com/definition/pascalcase). Example: 
```
`void ThisIsAFunction();
```


Variables are named using [snake_case](https://en.wikipedia.org/wiki/Snake_case). Example:
```
int this_is_a_variable = 0
```

## Identing Style
Identing is done using [Allman style](https://en.wikipedia.org/wiki/Indentation_style#Allman_style). Example:
```
void ThisIsAFunction()
{
    // Do something
}
```
## Class members declaration
Class methods and member declarations must be splitted in 4 categories in the following order: `public` methods, `private` methods, `public` members, and `private` members. If one of these categories doesn't exist, it should be omitted. Example:
```
public class StyleClass
{
public:
    StyleClass();
    ~StyleClass();
    
    void PublicMethod();
    
private:
    void PrivateMethod();
    
public:
    int public_member = 0;

private:
    int private_member = 0;
};
```

## Include order
Includes have to be split in three different blocks: **Engine includes**, **Library includes** and **C++ includes**. Each block have to be sorted alphabetically. Also, they should be written using absolute paths.
 
### Engine includes
Engine includes define `#include` pragmas for our own classes. So, everything under `Engine` folder should be in this block. Also, when including in a `.cpp` file the header file should be the first one included.
 
 ### Library includes
Library includes define `#include` pragmas for all imported libraries, excluding C++ and `std` common libraries. So, libraries like `MathGeoLib` and `assimp` should be there. Also, they are defined using `<>`.

### C++ includes
C++ includes define `#include` pragmas for C++ and `std` libraries. So, libraries like `vector` and `stdio` should be there. Also, they are defined using `<>`.

Here you have an example of a full include section. Take in account that if any block is to big it could be split in subsections.

```
#include "PanelConfiguration.h"

#include "Main/Application.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleWindow.h"

#include "Component/ComponentCamera.h"

#include <FontAwesome5/IconsFontAwesome5.h>
#include <GL/glew.h>
#include <imgui.h>
#include <SDL/SDL.h>
```

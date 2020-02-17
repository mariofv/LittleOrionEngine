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
bug/fix-stupid-bug
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

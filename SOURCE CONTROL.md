
# SOURCE CONTROL

## Overview
https://code.visualstudio.com/docs/sourcecontrol/overview
### Common workflows

#### Review changes before committing
Click on **Commit** 

#### Resolve merge conflicts
Options to resolve the conflicts:
* Use inline editor actions to choose how to resolve the conflicts directly in the editor
* Use the 3-way merge editor for a side-by-side view of changes and merge result
* Use AI assistance to help resolve merge conflicts

## Staging and committing
https://code.visualstudio.com/docs/sourcecontrol/staging-commits
### Staging changes

#### Stage specific lines or code blocks
Partial staging from the diff editor:
* Select the lines you want to stage
* Select a file in the Changes list to open the diff editor
* Use the Stage button in the gutter of the diff editor next to your selection to stage only those lines

### Commit your changes

#### Write commit messages
To cycle through your previous commit messages, press **Up** and **Down** while focused in the commit message input box.

#### Use the editor for commit messages
In the Source Control view, select Commit without entering a message in the commit input box. 

#### Undo the last commit
**Undo Last Commit** in **More Actions (...)** menu removes the last commit from branch history but keeps all the changes from that commit staged in the **Staged Changes** section

### Graph view for commit history
Select a commit and compare it with another branch or tag by right-clicking the commit and selecting **Compare with**, **Compare with Remote**, or **Compare with Merge Base**

## Branches and Worktrees
https://code.visualstudio.com/docs/sourcecontrol/branches-worktrees
### Working with branches

#### Rename and delete branches
* **Git: Rename Branch** from the Command Palette
* **Git: Delete Branch** from the Command Palette

#### Merge and publish branches
*  **Git: Merge Branch** from the Command Palette

### Working with Git worktrees

#### Understanding worktrees
Worktrees allows you to have multiple working directories for the same repository, each on a different branch.

Useful for:

* Work on multiple features simultaneously in separate folders
* Run different versions of your application side by side
* Compare implementations across branches

#### Create a worktree
* Select **Repositories** in **SOURCE CONTROL** more actions...
* Seect **Create worktree** in current repository more actions...

#### Switch between worktrees
Each worktree appears as a separate repository in the Source Control Repositories view

#### Compare and migrate changes from a worktree
* In the Source Control view, right-click a changed file in the worktree and select Compare with Workspace to see the differences side-by-side.
* After reviewing, use the Migrate Worktree Changes command from the Command Palette to merge all changes from a worktree into your current workspace.

##№ Working with GitHub in VS Code

### Editor integration

#### Hovers
A user can be mensioned as @lsv62 (for example, in a code comment), you can hover over that username and see a GitHub-style hover with the user's details. 

A similar hover for #-mentioned issue numbers, full GitHub issue URLs, and repository specified issues.
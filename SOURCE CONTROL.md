
# SOURCE CONTROL
https://code.visualstudio.com/docs/sourcecontrol/overview

## Source Control in VS Code

### Common workflows

#### Stage and commit changes
AI can generate a commit message based on your staged changes

#### Resolve merge conflicts
Options to resolve the conflicts:
* Use inline editor actions to choose how to resolve the conflicts directly in the editor
* Use the 3-way merge editor for a side-by-side view of changes and merge result
* Use AI assistance to help resolve merge conflicts

## Branches and Worktrees

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

## Working with GitHub in VS Code

### Editor integration

#### Hovers
A user can be mensioned as @lsv62 (for example, in a code comment), you can hover over that username and see a GitHub-style hover with the user's details. 

A similar hover for #-mentioned issue numbers, full GitHub issue URLs, and repository specified issues.
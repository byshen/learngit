# Git
Git is a version control system.


### Common commands:
- `git status`: untracked - unmodified - modified - staged
- `git add <file>`: begin to track file, 
- `git add --all`: begin to add all changes for commit
- ignore files through edit .gitignore file.
```
*.[oa]
build/
```

- `git diff`: look at changes not commited.
- `git diff --staged`: find the changed files that are not committed(first you need to add the files, and not commit it)
- `git commit -a -m "comments"`: stage every file and commit with comments. Best command ever
- `git rm <file>`: 

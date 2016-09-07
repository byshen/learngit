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
- `git commit -a -m "comments"`: stage every tracked file and commit with comments. Best command ever
- `git commit --amend`: used when you changed some file after commit and do not want to commit again.
- `git rm <file>`: `--cached` stop track the file, `--force` also delete it from the disk and not able to recover it from the trash.
- `git log -p -2`: view the changes of last two commit and log
- `git log --stat`: view all.
- `git reset HEAD <file>`: to unstage file.
- `git checkout -- <filename>`:dangerous one. Do not use.

### Some problems I met and solutions
- I use `git rm` to remove a file and found it is removed from disk. How to recover it ?
`git fetch`
`git checkout origin/master <file>` 

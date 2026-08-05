# Contributing to BING (Bing Is Not GNU)

Thank you for your interest in contributing to **BING**! We are building a modern, lightweight, and GPL-free alternative to standard GNU coreutils. Since this project is in its early stages, there is plenty of room for new features, utilities, and optimization.

## 🚀 How Can I Help?

### 1. Code Quality & Requirements
To ensure the project stays true to its philosophy, every contribution must follow these rules:
* **Supported Languages:** You can implement utilities using **Pure C** (standard C/POSIX system calls), **Rust** (without heavy dependencies), or **Go (Golang)**.
* **C++ Policy:** Using **C++** is generally discouraged. However, if you choose to use it, you must write in a strict **"C with Classes"** style. The standard template library (`std::`) is completely banned to avoid binary bloat. Avoid features like exceptions, RTTI, `<iostream>`, or heavy containers. Focus on clean, static memory boundaries.
* **Go (Golang) Rule:** If you write utilities in Go, you must explicitly disable or strictly restrict the Garbage Collector (e.g., using `debug.SetGCPercent(-1)` inside the code or compiling with `GOGC=off`). Since our utilities are short-lived, we rely on the OS to clean up memory upon exit, eliminating runtime GC overhead.
* **Zero Bloat:** Keep code minimal, high-performance, and secure. Avoid unnecessary third-party dependencies or heavy non-POSIX frameworks.
* **BSD 3-Clause License:** All contributions will be licensed under the permissive BSD 3-Clause license. No GPL-encumbered code is allowed.
* **Code Style & Safety:** Keep implementation headers clean. When using C/C++, avoid risky dynamic memory allocation (`malloc`/`new`) where static boundaries (like `4096` bytes for paths) are safer/faster, and use standard error handling (`perror`, `errno`, `stderr`). When using Rust or Go, leverage their built-in safety features and keep external dependencies to an absolute minimum to preserve small binary sizes.
* **OTHER LANGUAGES ARE PROHIBITED.**

### 🧪 2. Testing & Verification
Before submitting a Pull Request, you must verify your utility:
* **No Memory Leaks:** If your utility allocates memory, run it through `valgrind --leak-check=full` to ensure there are no leaks.
* **Standard Flags:** C/C++ utilities must compile without warnings using `-Wall -Wextra -O2`.
* **Behavior Check:** Verify your tool against the standard POSIX behavior. Ensure edge cases (like missing arguments or empty pipes) fail predictably and return correct non-zero exit codes.

### 📝 3. Commit Message Guidelines
We follow standard semantic commit messages to keep our git log clean:
* `feat: ...` for adding a completely new utility (e.g., `feat: implement cpwd utility`).
* `fix: ...` for fixing a bug in an existing tool.
* `docs: ...` for documentation updates.
* `perf: ...` for optimizations that improve binary size or speed.

## 🛠️ Contribution Workflow

1. **Fork** the repository and clone it locally.
2. Create a new branch for your feature: `git checkout -b feat-new-utility`.
3. Write your code and make sure it compiles flawlessly.
4. Commit your changes with descriptive messages: `git commit -m "feat: implement cpwd utility"`.
5. Push to your fork and open a **Pull Request** back to the main repository.

Let's build a fast, clean core utility library together!

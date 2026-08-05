# Contributing to BING (Bing Is Not GNU)

Thank you for your interest in contributing to **BING**! We are building a modern, lightweight, and GPL-free alternative to standard GNU coreutils. Since this project is in its early stages, there is plenty of room for new features, utilities, and optimization.

## 🚀 How Can I Help?

### 1. Implement New Utilities
We are looking for clean-room implementations of standard POSIX utilities. Some of the high-priority tools needed next:
* **`cpwd`** — Print working directory.
* **`cmkdir` / `crmdir`** — Create and remove directories.
* **`ctouch`** — Change file timestamps / create empty files.
* **`cuname`** — Print system information.

### 2. Code Quality & Requirements
To ensure the project stays true to its philosophy, every contribution must follow these rules:
* **Supported Languages:** You can implement utilities using **Pure C** (standard C/POSIX system calls), **Rust** (without heavy dependencies), or **Zig**.
* **Zero Bloat:** Keep code minimal, high-performance, and secure. Avoid unnecessary third-party dependencies or heavy non-POSIX frameworks.
* **BSD 3-Clause License:** All contributions will be licensed under the permissive BSD 3-Clause license. No GPL-encumbered code is allowed.
* **Code Style & Safety:** Keep implementation headers clean. When using C, avoid risky dynamic memory allocation (`malloc`) where static boundaries (like `4096` bytes for paths) are safer/faster, and use standard error handling (`perror`, `errno`, `stderr`). When using Rust or Zig, leverage their built-in safety features while keeping the binary size minimal.

## 🛠️ Contribution Workflow

1. **Fork** the repository and clone it locally.
2. Create a new branch for your feature: `git checkout -b feat-new-utility`.
3. Write your code and make sure it compiles flawlessly.
4. Commit your changes with descriptive messages: `git commit -m "feat: implement cpwd utility"`.
5. Push to your fork and open a **Pull Request** back to the main repository.

Let's build a fast, clean core utility library together!

# B.I.N.G (Binary Is Not GNU)

A modern, lightweight alternative to GNU Coreutils, built from scratch with clean, permissive licenses.

---

## 📖 Overview

**B.I.N.G** is a unified collection of minimalistic, high-performance command-line utilities written in pure C, FASM and part of C With Classes(C++ Without STL). Designed for POSIX-compliant systems, it completely avoids the bloat, legacy baggage, and restrictive GPL licensing of traditional GNU implementations. So, Fuck You GNU! Linux belongs to Linus and the community-not your bloated legacy.

Licensed under permissive terms (such as the BSD 3-Clause License or Public Domain where appropriate), BING gives developers and system administrators full freedom to modify, redistribute, and integrate these tools into any pipeline without friction.

---

## 🧰 Included Utilities

* **`cing`** — A clean-room alternative to `cat` for high-performance stream copying and file reading.
* **`ctrue`** — A minimalist implementation of `true`.
* **`cfalse`** — A minimalist implementation of `false`.
* **`cecho`** — a clean-room implementation of `echo`.
* **`cpwd`** — A minimalist implementation of `pwd`.
* **`cid`** — A minimalist realization of `id`.
* **`ccommand`** — A clean-room, standalone POSIX alternative to the shell built-in command utility of `posix-command`.
* **`cmkdir`** —  A realization of `mkdir`.
* **`csleep`** — a minimalist implementation of `sleep`.

---

## ✨ Core Features

* **Zero Bloat:** Stripped of unnecessary legacy layers, focusing purely on raw performance and simplicity.
* **Permissive Licensing:** Fully open-source under BSD 3-Clause and Public Domain terms.
* **Standard-Compliant:** Designed to behave predictably in standard POSIX environments.
* **Global or Local Installation:** Easily build and install utilities directly into your system or local binary paths.

---

## 🛠️ Installation & Tutorial

Clone the B.I.N.G repository and install the utilities on your Linux system.

### Step 1: Clone the Repository
Open your terminal and clone the project from GitHub:
```bash
git clone https://github.com/gpinteractive1-sudo/BING.git
cd BING

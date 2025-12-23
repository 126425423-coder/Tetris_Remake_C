# 🎮 Tetris Remake in C++

<div align="center">

[![GitHub stars](https://img.shields.io/github/stars/126425423-coder/Tetris_Remake_C?style=for-the-badge)](https://github.com/126425423-coder/Tetris_Remake_C/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/126425423-coder/Tetris_Remake_C?style=for-the-badge)](https://github.com/126425423-coder/Tetris_Remake_C/network)
[![GitHub issues](https://img.shields.io/github/issues/126425423-coder/Tetris_Remake_C?style=for-the-badge)](https://github.com/126425423-coder/Tetris_Remake_C/issues)
[![GitHub license](https://img.shields.io/github/license/126425423-coder/Tetris_Remake_C?style=for-the-badge)](LICENSE)

**A classic arcade game faithfully re-implemented in C++ as a console application.**

</div>

## 📖 Overview

This project is a direct remake of the iconic Tetris game, built entirely in C++. It aims to provide a one-to-one copy of the original gameplay experience through a simple console interface, focusing on core mechanics suchitive user input and score tracking.

## ✨ Features

-   🎯 **Classic Tetris Gameplay**: Experience the timeless block-dropping puzzle game.
-   🧱 **Seven Tetromino Shapes**: All standard I, O, T, S, Z, J, L pieces are included.
-   🔄 **Block Rotation**: Rotate falling pieces to fit into the grid.
-   ↔️ **Lateral Movement**: Move pieces left and right across the playfield.
-   ⬇️ **Soft and Hard Drop**: Control the descent speed or instantly drop pieces.
-   💥 **Line Clearing**: Clear complete horizontal lines to score points.
-   📈 **Score Tracking**: Keep track of your high scores.
-   🛑 **Game Over Condition**: Ends the game when pieces stack to the top.
-   ⌨️ **Keyboard Controls**: Intuitive controls for an engaging gameplay experience.

## 🖥️ Screenshots

<!-- TODO: Add actual screenshots of the console game in action. -->
<img width="625" height="750" alt="Tetris_Remake_efeccypn08" src="https://github.com/user-attachments/assets/fe9488b1-2e84-464f-a28d-c4b65705e5e7" />
<img width="625" height="750" alt="Tetris_Remake_QD8GPxmJJk" src="https://github.com/user-attachments/assets/1f3f14c1-c308-4c0a-8a5c-8e8e007bdc92" />
<img width="625" height="750" alt="Tetris_Remake_Wg1kSOs15a" src="https://github.com/user-attachments/assets/5188cdf1-ca44-4a6f-82be-77dc75c4487c" />
<img width="625" height="750" alt="Tetris_Remake_26ol0Q1V7n" src="https://github.com/user-attachments/assets/5c922bb2-cc26-400d-91d1-bce65d58ef3e" />


## 🛠️ Tech Stack

**Language:**
[![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)

**Build System:**
-   **GCC/G++**: Standard C++ Compiler for building the executable.

## 🚀 Quick Start

### Prerequisites

To compile and run this project, you will need a C++ compiler installed on your system.
-   **GCC/G++** (recommended for Linux/macOS, available via MinGW on Windows)
-   **Clang**
-   **MSVC** (Visual Studio C++ compiler on Windows)

### Installation & Build

1.  **Clone the repository**
    ```bash
    git clone https://github.com/126425423-coder/Tetris_Remake_C.git
    cd Tetris_Remake_C
    ```

2.  **Compile the source code**
    Using GCC/G++:
    ```bash
    g++ tetris_remake.cpp -o tetris_remake -std=c++17 -Wall
    ```
    *   `-o tetris_remake`: Specifies the output executable name.
    *   `-std=c++17`: Compiles using the C++17 standard (ensure your compiler supports it, or adjust if needed).
    *   `-Wall`: Enables all common warning messages.

3.  **Run the game**
    ```bash
    ./tetris_remake
    ```
    *(On Windows, you might run `tetris_remake.exe` directly)*

## 🎮 How to Play

Once the game is running in your console, use the following keyboard controls:

| Key      | Action            |
| :------- | :---------------- |
| `Left Arrow`  | Move piece left   |
| `Right Arrow` | Move piece right  |
| `Up Arrow`    | Rotate piece      |
| `Down Arrow`  | Soft drop (speed up fall) |
| `Spacebar`    | Hard drop (instant fall) |
| `Q` / `Esc`   | Quit game         |

## 📁 Project Structure

```
Tetris_Remake_C/
├── tetris_remake.cpp # Main source code file for the game logic and rendering.
└── README.md         # This README file.
```

## 🤝 Contributing

We welcome contributions to enhance this Tetris remake! If you have suggestions or want to improve the codebase, please feel free to:

1.  Fork the repository.
2.  Create a new branch (`git checkout -b feature/AmazingFeature`).
3.  Commit your changes (`git commit -m 'Add some AmazingFeature'`).
4.  Push to the branch (`git push origin feature/AmazingFeature`).
5.  Open a Pull Request.

### Development Setup for Contributors

The development setup is straightforward: simply ensure you have a C++ compiler and compile the `tetris_remake.cpp` file as described in the [Installation & Build](#installation-build) section.

## 📄 License

This project is licensed under the **[No License Specified]** - please check the repository for any `LICENSE` file that might be added in the future.

## 🙏 Acknowledgments

-   Inspired by the classic Tetris game developed by Alexey Pajitnov.

## 📞 Support & Contact

If you encounter any issues or have questions, please use the GitHub Issues page:
-   🐛 Issues: [GitHub Issues](https://github.com/126425423-coder/Tetris_Remake_C/issues)


The project is written in procedural C-style code and compiled as C++ due to the use of the .cpp extension and MSVC toolchain.

---

<div align="center">

**⭐ Star this repo if you find it helpful or fun!**

Made with ❤️ by [126425423-coder](https://github.com/126425423-coder)

</div>

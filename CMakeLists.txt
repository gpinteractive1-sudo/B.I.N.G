cmake_minimum_required(VERSION 3.10)
project(BING VERSION 1.0.0 LANGUAGES C)

# Встановлюємо суворий стандарт C99 та декларуємо Full POSIX 2008
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_definitions(_POSIX_C_SOURCE=200809L)

# Додаємо прапорці попереджень компілятора та оптимізацію швидкодії O2
add_compile_options(-Wall -Wextra -O2)

# Налаштовуємо канонічний шлях встановлення в систему за замовчуванням (/usr/local/bin)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "..." FORCE)
endif()

# Підключаємо вкладену папку з вашими Сі-утилітами
add_subdirectory(BING_Utils/c)

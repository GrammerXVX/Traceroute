Простая реализация утилиты `traceroute` на C++, использующая ICMP через `SOCK_RAW`. Определяет маршрут к целевому IP или доменному имени, с таймингами, разрешением доменных имён и форматированным выводом.

---

## 📦 Требования

- **Windows**
- **Компилятор C++**, например Visual Studio 2022
- **Отключить Брандмауэр** для общедоступных сетей\частных сетей
- **Администраторские права** (для создания RAW-сокета)

---

## ⚙️ Сборка

> Используйте Visual Studio 2022:
1. Открыть `traceroute.sln`
2. Собрать проект в Release/Debug
3. Запустить .exe **от имени администратора**

Или в консоли (для `cl.exe`):

```bash
cl src\\main.cpp src\\Traceroute.cpp /Fe:traceroute.exe /I src /link ws2_32.lib iphlpapi.lib
```

---

## ⚙️ Тесты

>**✅Тест**
- Ввод ip ```9.9.9.9```

![Тест-Good](https://github.com/user-attachments/assets/13132a98-45a6-4dbf-ba5e-acea86780443)

---
>**✅Тест**
- Ввод домена ```google.com```

![Тест-Good-2](https://github.com/user-attachments/assets/36094ce1-2f1f-4d3f-9068-ad9a85eaeb7d)

---
>**✅Тест**
- Ввод неправильного адреса ```inccorect.com```

![Inccorect-adress](https://github.com/user-attachments/assets/858be5f9-f52d-422a-a395-2f6d839cfc2f)

---
>**✅Тест**
- При включенном ```Брандмауэр```
  
![Defence](https://github.com/user-attachments/assets/6f875fea-939c-48e8-9910-95746fd1da6d)

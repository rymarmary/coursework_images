# coursework_images

Программа должна иметь CLI или GUI. Более подробно тут: http://se.moevm.info/doku.php/courses:programming:rules_extra_kurs

Программа должна реализовывать весь следующий функционал по обработке bmp-файла
Общие сведения:
- 24 бита на цвет
- без сжатия
- файл всегда соответствует формату BMP (но стоит помнить, что версий у формата несколько)
- обратите внимание на выравнивание; мусорные данные, если их необходимо дописать в файл для выравнивания, должны быть нулями.
- обратите внимание на порядок записи пикселей
- все поля стандартных BMP заголовков в выходном файле должны иметь те же значения что и во входном (разумеется кроме тех, которые должны быть изменены).

Программа должна реализовывать следующий функционал по обработке bmp-файла:

(1) Рисование отрезка. Отрезок определяется:
- координатами начала
- координатами конца
- цветом
- толщиной

(2) Инвертировать цвета в заданной окружности. 
- Окружность определяется либо координатами левого верхнего и правого нижнего угла квадрата, в который она вписана, либо координатами ее центра и радиусом

(3) Обрезка изображения. Требуется обрезать изображение по заданной области. Область определяется:
- Координатами левого верхнего угла 
- Координатами правого нижнего угла 

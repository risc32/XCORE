set osabi none
file build/kernel.elf
python
import gdb.printing

class StringPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        data = self.val['_data']
        size = self.val['_size']

        if data:
            try:
                # Читаем ровно size символов
                string_val = data.string(length=size)
                return '\"'+string_val+'\"'
            except:
                return f"<error reading string>"
        return "null"

    def children(self):
            """Это отображается при раскрытии (expand)"""
            data = self.val['_data']
            size = self.val['_size']
            capacity = self.val['_capacity']
            nullend = self.val['nullend']

            # Показываем сырые данные
            yield ('_data', data)
            yield ('_size', size)
            yield ('_capacity', capacity)
            yield ('nullend', nullend)

    def display_hint(self):
        return 'string'

# Регистрируем pretty printer
def register_printers():
    printer = gdb.printing.RegexpCollectionPrettyPrinter("my_project")
    printer.add_printer('string', '^string$', StringPrinter)
    printer.add_printer('wstring', '^wstring$', StringPrinter)

    gdb.printing.register_pretty_printer(None, printer)

register_printers()
end
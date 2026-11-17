"""Интеграционные тесты: запускаем dbms как пользователь, проверяем файлы и вывод."""
import os
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

APP = str(Path(sys.argv[1]).resolve()) if len(sys.argv) > 1 else "./dbms"
sys.argv = sys.argv[:1]


class TestDBMS(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)

    def command(self, filename, query, expected_returncode=0):
        completed = subprocess.run(
            [APP, "--file", str(filename), "--query", query],
            capture_output=True,
            text=True,
            encoding="utf-8",
            timeout=10,
            check=False,
        )
        self.assertEqual(completed.returncode, expected_returncode, completed.stdout + completed.stderr)
        return completed.stdout

    def test_binary_array_roundtrip_with_spaces(self):
        path = Path(self.temp.name) / "base.bin"
        self.command(path, 'MPUSH products "hello world"')
        self.command(path, 'MPUSH products "second line"')
        self.assertTrue(path.read_bytes().startswith(b"LB3B"))
        self.assertEqual(self.command(path, "MGET products 0"), "hello world\n")
        self.assertEqual(self.command(path, "MGET products 1"), "second line\n")
        self.assertEqual(self.command(path, "MLEN products"), "2\n")

    def test_binary_all_container_types(self):
        file = Path(self.temp.name) / "all.bin"
        cases = [
            ("MPUSH arr a", "PRINT arr", "a"),
            ("FPUSHTAIL one a", "PRINT one", "a"),
            ("LPUSHTAIL two a", "PRINT two", "a"),
            ("SPUSH st a", "PRINT st", "a"),
            ("QPUSH que a", "PRINT que", "a"),
            ("TINSERT tree 17", "TGET tree 17", "TRUE"),
            ("CPUT chain key value", "CGET chain key", "value"),
            ("KPUT cuckoo key value", "KGET cuckoo key", "value"),
        ]
        for write, read, expected in cases:
            self.command(file, write)
            self.assertIn(expected, self.command(file, read))
        self.assertTrue(file.read_bytes().startswith(b"LB3B"))

    def test_binary_unicode_and_truncated_file(self):
        path = Path(self.temp.name) / "utf8.bin"
        self.command(path, 'MPUSH arr "Привет Linux"')
        self.assertEqual(self.command(path, "MGET arr 0"), "Привет Linux\n")

        # Повреждённый файл должен отклоняться, а не перезаписываться.
        bad = path.read_bytes()[:-1]
        path.write_bytes(bad)
        output = self.command(path, "MPUSH arr second", expected_returncode=1)
        self.assertIn("Ошибка:", output)
        self.assertEqual(path.read_bytes(), bad)

    def test_text_compatibility(self):
        file = Path(self.temp.name) / "old.data"
        self.command(file, "MPUSH arr hello")
        self.assertEqual(file.read_text(encoding="utf-8"), "M arr hello\n")
        self.assertEqual(self.command(file, "MGET arr 0"), "hello\n")

    def test_bad_binary_rejected_without_overwrite(self):
        file = Path(self.temp.name) / "invalid.bin"
        original = b"INVALID BINARY"
        file.write_bytes(original)
        out = self.command(file, "MPUSH arr hello", expected_returncode=1)
        self.assertIn("Ошибка:", out)
        self.assertEqual(file.read_bytes(), original)


if __name__ == "__main__":
    unittest.main()

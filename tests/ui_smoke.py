"""Smoke test UI stud_sys: program w pseudoterminalu, ekran przez pyte.

usage: ui_smoke.py <binary> <PL|EN>

Scenariusz: menu glowne -> Budzik (dodanie, wpis nazwy, zapis, usuniecie)
-> Stan systemu -> wyjscie Esc z kodem 0. HOME i katalog roboczy to
katalog tymczasowy, wiec test nie dotyka plikow uzytkownika.
"""
import os
import pty
import select
import shutil
import signal
import struct
import sys
import tempfile
import time
from typing import NoReturn

import pyte

COLS, ROWS = 100, 36
KEYS = {"ENTER": b"\r", "ESC": b"\x1b", "DOWN": b"\x1b[B", "UP": b"\x1b[A"}

TXT = {
    "PL": {"menu": "MENU GLOWNE", "budzik": "Budzik", "lista": "LISTA BUDZIKOW",
           "dodaj": "+++ Dodaj nowy budzik +++", "nowy": "Wpisz do agendy",
           "usun": "Usun budzik", "stan": "STAN SYSTEMU"},
    "EN": {"menu": "MAIN MENU", "budzik": "Alarm", "lista": "ALARM LIST",
           "dodaj": "+++ Add new alarm +++", "nowy": "Save to agenda",
           "usun": "Delete alarm", "stan": "SYSTEM STATUS"},
}


class Program:
    def __init__(self, binary, home):
        self.screen = pyte.Screen(COLS, ROWS)
        self.stream = pyte.ByteStream(self.screen)
        self.pid, self.fd = pty.fork()
        if self.pid == 0:
            os.chdir(home)
            env = dict(os.environ, HOME=home, TERM="xterm",
                       LINES=str(ROWS), COLUMNS=str(COLS))
            os.execve(binary, [binary], env)
        import fcntl
        import termios
        fcntl.ioctl(self.fd, termios.TIOCSWINSZ,
                    struct.pack("HHHH", ROWS, COLS, 0, 0))
        self.status = None

    def pump(self, sec):
        end = time.time() + sec
        while time.time() < end:
            r, _, _ = select.select([self.fd], [], [], 0.05)
            if not r:
                continue
            try:
                data = os.read(self.fd, 65536)
            except OSError:
                return
            if not data:
                return
            self.stream.feed(data)

    def text(self):
        return "\n".join(line.rstrip() for line in self.screen.display)

    def wait_for(self, needle, timeout=5.0):
        end = time.time() + timeout
        while time.time() < end:
            if needle in self.text():
                return
            self.pump(0.1)
        fail(f"nie pojawilo sie na ekranie: {needle!r}", self)

    def wait_gone(self, needle, timeout=5.0):
        end = time.time() + timeout
        while time.time() < end:
            if needle not in self.text():
                return
            self.pump(0.1)
        fail(f"nadal na ekranie: {needle!r}", self)

    def key(self, name, settle=0.3):
        os.write(self.fd, KEYS[name])
        self.pump(settle)

    def type(self, text):
        for ch in text:
            os.write(self.fd, ch.encode())
            self.pump(0.05)

    def wait_exit(self, timeout=5.0):
        end = time.time() + timeout
        while time.time() < end:
            self.pump(0.1)
            pid, status = os.waitpid(self.pid, os.WNOHANG)
            if pid:
                self.status = status
                return status
        return None

    def kill(self):
        if self.status is None:
            try:
                os.kill(self.pid, signal.SIGKILL)
                os.waitpid(self.pid, 0)
            except (ProcessLookupError, ChildProcessError):
                pass


def fail(msg, prog=None) -> NoReturn:
    print(f"FAIL: {msg}")
    if prog is not None:
        print("----- ekran -----")
        print(prog.text())
        prog.kill()
    sys.exit(1)


def main():
    binary, lang = os.path.abspath(sys.argv[1]), sys.argv[2].upper()
    t = TXT[lang]
    home = tempfile.mkdtemp(prefix="stud_sys_ui_")
    plik = os.path.join(home, ".stud_sys_budz.dat")
    prog = Program(binary, home)
    try:
        # menu glowne
        prog.wait_for(t["menu"])
        prog.wait_for(t["budzik"])

        # Budzik: pusta lista
        prog.key("ENTER")
        prog.wait_for(t["lista"])
        prog.wait_for(t["dodaj"])

        # nowy budzik: nazwa przez linie wpisu, zapis do agendy
        prog.key("ENTER")
        prog.wait_for(t["nowy"])
        prog.key("ENTER")             # pole Sprawa
        prog.type("Smoke")            # pierwszy znak zastepuje stara wartosc
        prog.key("ENTER")
        prog.wait_for("Smoke")
        for _ in range(3):
            prog.key("DOWN", 0.15)    # >>> Wpisz do agendy <<<
        prog.key("ENTER")
        prog.wait_gone(t["nowy"])
        prog.wait_for("Smoke")        # wpis na liscie
        if not os.path.exists(plik):
            fail("brak pliku budzikow po zapisie", prog)

        # odliczanie na liscie: scheduler dziala, gdy program czeka na klawisz
        before = prog.text()
        prog.pump(2.2)
        if prog.text() == before:
            fail("lista budzikow nie odswieza sie (odliczanie stoi)", prog)

        # usuniecie budzika
        prog.key("ENTER")             # Smoke (pozycja 0)
        prog.wait_for(t["usun"])
        for _ in range(4):
            prog.key("DOWN", 0.15)    # >>> Usun budzik <<<
        prog.key("ENTER")
        prog.wait_gone("Smoke")

        # Stan systemu
        prog.key("ESC")
        prog.wait_for(t["menu"])
        prog.key("DOWN", 0.15)
        prog.key("DOWN", 0.15)
        prog.key("ENTER")
        prog.wait_for(t["stan"])
        prog.key("ESC")
        prog.wait_for(t["menu"])

        # wyjscie z programu
        prog.key("ESC")
        status = prog.wait_exit()
        if status is None:
            fail("program nie zakonczyl sie po Esc w menu glownym", prog)
        if not os.WIFEXITED(status) or os.WEXITSTATUS(status) != 0:
            fail(f"zly kod wyjscia: {status}", prog)
    finally:
        prog.kill()
        shutil.rmtree(home, ignore_errors=True)
    print(f"ui smoke ({lang}): OK")


if __name__ == "__main__":
    main()

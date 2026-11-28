"""Test pytania <t/n> silnika formularzy (potwierdz_tn) w pseudoterminalu.

usage: ui_tn.py <tn_probe>

tn_probe zadaje trzy pytania na tle "0123456789ABCDEFGHIJ" (wiersz 2,
kolumna 4). Sprawdzamy: tekst pytania, obciecie do 3 znakow, odtworzenie
tla, ignorowanie innych klawiszy oraz wyniki 'n', Esc i 't'.
"""
import os
import shutil
import sys
import tempfile

from ui_smoke import Program, fail

TLO = "0123456789ABCDEFGHIJ"


def wiersz(prog, y):
    return prog.screen.display[y].rstrip()


def czekaj_wiersz(prog, y, oczekiwany, timeout=5.0):
    import time
    end = time.time() + timeout
    while time.time() < end:
        if wiersz(prog, y) == oczekiwany:
            return
        prog.pump(0.1)
    fail(f"wiersz {y}: {wiersz(prog, y)!r}, oczekiwany {oczekiwany!r}", prog)


def main():
    binary = os.path.abspath(sys.argv[1])
    home = tempfile.mkdtemp(prefix="stud_sys_tn_")
    prog = Program(binary, home)
    try:
        prog.wait_for("PYTANIE1")
        czekaj_wiersz(prog, 2, "0123<t/n>? BCDEFGHIJ")
        prog.type("x")                       # inny klawisz: pytanie zostaje
        prog.pump(0.3)
        czekaj_wiersz(prog, 2, "0123<t/n>? BCDEFGHIJ")
        prog.type("n")

        prog.wait_for("PYTANIE2")
        czekaj_wiersz(prog, 4, "R1=n")
        czekaj_wiersz(prog, 2, "0123<t/789ABCDEFGHIJ")   # obciete do 3 znakow
        prog.key("ESC")

        prog.wait_for("PYTANIE3")
        czekaj_wiersz(prog, 5, "R2=E")
        czekaj_wiersz(prog, 2, "0123<t/n>? BCDEFGHIJ")
        prog.type("t")

        prog.wait_for("KONIEC")
        czekaj_wiersz(prog, 6, "R3=t")
        czekaj_wiersz(prog, 2, TLO)          # tlo odtworzone po pytaniu
        prog.type("q")
        status = prog.wait_exit()
        if status is None or not os.WIFEXITED(status) or os.WEXITSTATUS(status) != 0:
            fail(f"zly koniec tn_probe: {status}", prog)
    finally:
        prog.kill()
        shutil.rmtree(home, ignore_errors=True)
    print("ui t/n: OK")


if __name__ == "__main__":
    main()

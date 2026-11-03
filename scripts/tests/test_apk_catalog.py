"""Regression coverage for the imported content and stopped-deployment repairs."""
import json
import hashlib
from pathlib import Path
import sys
import tempfile
import re
import unittest
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
from import_apk_catalog import decode, merge_table, merge_xml


class CatalogRegression(unittest.TestCase):
    def setUp(self):
        self.catalog = json.loads((ROOT / "resources/catalog/v330.json").read_text(encoding="utf-8"))

    def test_fate_extra_column_repaired_without_replacing_legacy_rows(self):
        spec = self.catalog["serverTemplates"]["fate.xml"]
        before = spec["previousRows"]["937"]
        after = next(row for row in spec["rows"] if row["id"] == "937")
        with tempfile.TemporaryDirectory() as directory:
            xml = Path(directory) / "fate.xml"
            old = '<Fate id="1" type="1" require="41" upgradeType="3" quantity="0.25"/>'
            damaged = '<Fate ' + ' '.join(key + '="' + value + '"' for key, value in before.items()) + '/>'
            xml.write_text('<root>\n\t<Fates>\n' + old + damaged + '\n\t</Fates>\n</root>', encoding="utf-8")
            merge_xml(xml, {"container": "Fates", "tag": "Fate", "rows": [after], "previousRows": {"937": before}})
            actual = ET.parse(xml).find("Fates/Fate[@id='937']")
            self.assertEqual(actual.get("upgradeType"), "3")
            self.assertEqual(actual.get("quantity"), "0.26")
            self.assertIn(old, xml.read_text(encoding="utf-8"))
            table = Path(directory) / "Yuanfens.txt"
            previous = self.catalog["clientPreviousRows"]["Yuanfens.txt"]
            row = next(row for row in self.catalog["clientTables"]["Yuanfens.txt"] if row[0] == "937")
            table.write_text('header\n' + '\t'.join(previous["937"]) + '\n', encoding="utf-8")
            merge_table(table, [row], previous)
            self.assertEqual(decode(table.read_bytes()).splitlines()[1].split(), row)

    def test_custom_same_id_is_not_overwritten_by_repair(self):
        spec = self.catalog["serverTemplates"]["fate.xml"]
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "fate.xml"
            original = b'<root><Fates><Fate id="937" type="1" require="19" upgradeType="3" quantity="0.99"/></Fates></root>'
            path.write_bytes(original)
            with self.assertRaises(ValueError):
                merge_xml(path, spec)
            self.assertEqual(path.read_bytes(), original)

    def test_published_fates_have_numeric_columns_and_match_server(self):
        plain = decode((ROOT / "resources/client/Yuanfens.txt").read_bytes())
        rows = {line.split()[0]: line.split() for line in plain.splitlines()[1:] if line.split()}
        self.assertEqual(len(rows), 940)
        for row in self.catalog["serverTemplates"]["fate.xml"]["rows"]:
            self.assertIn(int(row["upgradeType"]), (1, 2, 3, 4))
            self.assertGreater(float(row["quantity"]), 0)
            client = rows[row["id"]]
            self.assertEqual(len(client), 7)
            self.assertEqual(int(client[4]), int(row["upgradeType"]))
            self.assertAlmostEqual(float(client[5]) / 100, float(row["quantity"]))

    def test_imported_card_fate_ids_exist_on_both_sides(self):
        server = {row["id"] for row in self.catalog["serverTemplates"]["fate.xml"]["rows"]}
        self.assertEqual(len(self.catalog["serverTemplates"]["disciple.xml"]["rows"]), 44)
        for row in self.catalog["serverTemplates"]["disciple.xml"]["rows"]:
            for index in range(1, 7):
                self.assertTrue(row["fate" + str(index)] == "0" or row["fate" + str(index)] in server)
            for field in ("defaultPotentiality", "defaultHealth", "growAttack", "growDefence", "growHealth", "growRp"):
                self.assertGreater(float(row[field]), 0)

    def test_new_names_and_descriptions_are_supported_by_the_font(self):
        font = (ROOT / "resources/client/Lang/heiOutline24.fnt").read_text(encoding="utf-8")
        glyphs = {int(value) for value in re.findall(r"char id=(\d+)", font)}
        for name, rows in self.catalog["clientTables"].items():
            description = 6 if name == "Yuanfens.txt" else 4
            for row in rows:
                for character in row[1] + row[description]:
                    if ord(character) > 127:
                        self.assertIn(ord(character), glyphs, "%s #%s character %s" % (name, row[0], character))

    def test_imported_assets_have_exact_case_and_expected_content(self):
        folder = ROOT / "resources/client"
        actual_names = {path.relative_to(folder).as_posix() for path in (folder / "ported/v330").rglob("*") if path.is_file()}
        for name, expected in self.catalog["assets"].items():
            self.assertIn(name, actual_names)
            self.assertEqual(hashlib.sha256((folder / name).read_bytes()).hexdigest(), expected)
        for name, hashes in self.catalog["sharedAssets"].items():
            self.assertEqual(hashlib.sha256((folder / name).read_bytes()).hexdigest(), hashes["afterSha256"])


if __name__ == "__main__":
    unittest.main()

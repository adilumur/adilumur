-- remove (if exist) existing tables
DROP FUNCTION IF EXISTS remove_all();

-- Using PL/SQL create function to drop tables and sequences
CREATE or replace FUNCTION remove_all() RETURNS void AS $$
DECLARE
    rec RECORD;
    cmd text;
BEGIN
    cmd := '';

    FOR rec IN SELECT
            'DROP SEQUENCE ' || quote_ident(n.nspname) || '.'
                || quote_ident(c.relname) || ' CASCADE;' AS name
        FROM
            pg_catalog.pg_class AS c
        LEFT JOIN
            pg_catalog.pg_namespace AS n
        ON
            n.oid = c.relnamespace
        WHERE
            relkind = 'S' AND
            n.nspname NOT IN ('pg_catalog', 'pg_toast') AND
            pg_catalog.pg_table_is_visible(c.oid)
    LOOP
        cmd := cmd || rec.name;
    END LOOP;

    FOR rec IN SELECT
            'DROP TABLE ' || quote_ident(n.nspname) || '.'
                || quote_ident(c.relname) || ' CASCADE;' AS name
        FROM
            pg_catalog.pg_class AS c
        LEFT JOIN
            pg_catalog.pg_namespace AS n
        ON
            n.oid = c.relnamespace WHERE relkind = 'r' AND
            n.nspname NOT IN ('pg_catalog', 'pg_toast') AND
            pg_catalog.pg_table_is_visible(c.oid)
    LOOP
        cmd := cmd || rec.name;
    END LOOP;

    EXECUTE cmd;
    RETURN;
END;
$$ LANGUAGE plpgsql;
select remove_all();
-- End of removing

CREATE TABLE adresa (
    id_adresa SERIAL NOT NULL,
    id_stat INTEGER NOT NULL,
    ulice VARCHAR(256) NOT NULL,
    cislo_domu VARCHAR(256) NOT NULL,
    mesto VARCHAR(256) NOT NULL,
    pos VARCHAR(256) NOT NULL
);
ALTER TABLE adresa ADD CONSTRAINT pk_adresa PRIMARY KEY (id_adresa);

CREATE TABLE byt (
    id_byt SERIAL NOT NULL,
    id_adresa INTEGER NOT NULL,
    id_osoba INTEGER NOT NULL,
    patro VARCHAR(256) NOT NULL,
    cislo_bytu VARCHAR(256) NOT NULL
);
ALTER TABLE byt ADD CONSTRAINT pk_byt PRIMARY KEY (id_byt);

CREATE TABLE firma (
    id_firma SERIAL NOT NULL,
    nazev VARCHAR(256) NOT NULL
);
ALTER TABLE firma ADD CONSTRAINT pk_firma PRIMARY KEY (id_firma);

CREATE TABLE najemce (
    id_osoba INTEGER NOT NULL,
    trvale_bydliste VARCHAR(256),
    plat VARCHAR(256),
    kde_pracuje VARCHAR(256)
);
ALTER TABLE najemce ADD CONSTRAINT pk_najemce PRIMARY KEY (id_osoba);

CREATE TABLE osoba (
    id_osoba SERIAL NOT NULL,
    osobni_cislo VARCHAR(256) NOT NULL,
    jmeno VARCHAR(256) NOT NULL,
    prijmeni VARCHAR(256) NOT NULL,
    datum_narozeni VARCHAR(256) NOT NULL
);
ALTER TABLE osoba ADD CONSTRAINT pk_osoba PRIMARY KEY (id_osoba);
ALTER TABLE osoba ADD CONSTRAINT uc_osoba_osobni_cislo UNIQUE (osobni_cislo);

CREATE TABLE platba (
    datum_placeni SERIAL NOT NULL,
    id_smlouva_najemni INTEGER NOT NULL,
    mnozstvi VARCHAR(256) NOT NULL
);
ALTER TABLE platba ADD CONSTRAINT pk_platba PRIMARY KEY (datum_placeni, id_smlouva_najemni);

CREATE TABLE predmet (
    zkratka VARCHAR(256) NOT NULL,
    obtiznost VARCHAR(256) NOT NULL
);
ALTER TABLE predmet ADD CONSTRAINT pk_predmet PRIMARY KEY (zkratka);

CREATE TABLE smlouva_firma_najemce (
    id_smlouva_firma_najemce SERIAL NOT NULL,
    id_firma INTEGER NOT NULL,
    id_osoba INTEGER NOT NULL,
    datum_podepsani VARCHAR(256) NOT NULL
);
ALTER TABLE smlouva_firma_najemce ADD CONSTRAINT pk_smlouva_firma_najemce PRIMARY KEY (id_smlouva_firma_najemce);

CREATE TABLE smlouva_firma_vlastnik (
    id_smlouva_firma_vlastnik SERIAL NOT NULL,
    id_firma INTEGER NOT NULL,
    id_osoba INTEGER NOT NULL,
    datum_podepsani VARCHAR(256) NOT NULL
);
ALTER TABLE smlouva_firma_vlastnik ADD CONSTRAINT pk_smlouva_firma_vlastnik PRIMARY KEY (id_smlouva_firma_vlastnik);

CREATE TABLE smlouva_najemni (
    id_smlouva_najemni SERIAL NOT NULL,
    id_osoba INTEGER NOT NULL,
    id_byt INTEGER NOT NULL,
    vlastnik_id_osoba INTEGER NOT NULL,
    datum_podepsani VARCHAR(256) NOT NULL,
    datum_zacatek_pronajmu VARCHAR(256) NOT NULL,
    datum_konec_pronajmu VARCHAR(256) NOT NULL
);
ALTER TABLE smlouva_najemni ADD CONSTRAINT pk_smlouva_najemni PRIMARY KEY (id_smlouva_najemni);

CREATE TABLE stat (
    id_stat SERIAL NOT NULL,
    nazev VARCHAR(256) NOT NULL
);
ALTER TABLE stat ADD CONSTRAINT pk_stat PRIMARY KEY (id_stat);

CREATE TABLE student (
    usernam VARCHAR(256) NOT NULL,
    studium INTEGER NOT NULL,
    attribute VARCHAR(256) NOT NULL
);
ALTER TABLE student ADD CONSTRAINT pk_student PRIMARY KEY (usernam);

CREATE TABLE tahak (
    id_tahak INTEGER NOT NULL,
    zkratka VARCHAR(256) NOT NULL,
    usernam VARCHAR(256) NOT NULL,
    forma VARCHAR(256) NOT NULL
);
ALTER TABLE tahak ADD CONSTRAINT pk_tahak PRIMARY KEY (id_tahak);
ALTER TABLE tahak ADD CONSTRAINT u_fk_tahak_student UNIQUE (usernam);

CREATE TABLE vlastnik (
    id_osoba INTEGER NOT NULL
);
ALTER TABLE vlastnik ADD CONSTRAINT pk_vlastnik PRIMARY KEY (id_osoba);

CREATE TABLE zamestnanec (
    id_osoba INTEGER NOT NULL,
    id_firma INTEGER
);
ALTER TABLE zamestnanec ADD CONSTRAINT pk_zamestnanec PRIMARY KEY (id_osoba);

ALTER TABLE adresa ADD CONSTRAINT fk_adresa_stat FOREIGN KEY (id_stat) REFERENCES stat (id_stat) ON DELETE CASCADE;

ALTER TABLE byt ADD CONSTRAINT fk_byt_adresa FOREIGN KEY (id_adresa) REFERENCES adresa (id_adresa) ON DELETE CASCADE;
ALTER TABLE byt ADD CONSTRAINT fk_byt_vlastnik FOREIGN KEY (id_osoba) REFERENCES vlastnik (id_osoba) ON DELETE CASCADE;

ALTER TABLE najemce ADD CONSTRAINT fk_najemce_osoba FOREIGN KEY (id_osoba) REFERENCES osoba (id_osoba) ON DELETE CASCADE;

ALTER TABLE platba ADD CONSTRAINT fk_platba_smlouva_najemni FOREIGN KEY (id_smlouva_najemni) REFERENCES smlouva_najemni (id_smlouva_najemni) ON DELETE CASCADE;

ALTER TABLE smlouva_firma_najemce ADD CONSTRAINT fk_smlouva_firma_najemce_firma FOREIGN KEY (id_firma) REFERENCES firma (id_firma) ON DELETE CASCADE;
ALTER TABLE smlouva_firma_najemce ADD CONSTRAINT fk_smlouva_firma_najemce_najemc FOREIGN KEY (id_osoba) REFERENCES najemce (id_osoba) ON DELETE CASCADE;

ALTER TABLE smlouva_firma_vlastnik ADD CONSTRAINT fk_smlouva_firma_vlastnik_firma FOREIGN KEY (id_firma) REFERENCES firma (id_firma) ON DELETE CASCADE;
ALTER TABLE smlouva_firma_vlastnik ADD CONSTRAINT fk_smlouva_firma_vlastnik_vlast FOREIGN KEY (id_osoba) REFERENCES vlastnik (id_osoba) ON DELETE CASCADE;

ALTER TABLE smlouva_najemni ADD CONSTRAINT fk_smlouva_najemni_najemce FOREIGN KEY (id_osoba) REFERENCES najemce (id_osoba) ON DELETE CASCADE;
ALTER TABLE smlouva_najemni ADD CONSTRAINT fk_smlouva_najemni_byt FOREIGN KEY (id_byt) REFERENCES byt (id_byt) ON DELETE CASCADE;
ALTER TABLE smlouva_najemni ADD CONSTRAINT fk_smlouva_najemni_vlastnik FOREIGN KEY (vlastnik_id_osoba) REFERENCES vlastnik (id_osoba) ON DELETE CASCADE;

ALTER TABLE tahak ADD CONSTRAINT fk_tahak_predmet FOREIGN KEY (zkratka) REFERENCES predmet (zkratka) ON DELETE CASCADE;
ALTER TABLE tahak ADD CONSTRAINT fk_tahak_student FOREIGN KEY (usernam) REFERENCES student (usernam) ON DELETE CASCADE;

ALTER TABLE vlastnik ADD CONSTRAINT fk_vlastnik_osoba FOREIGN KEY (id_osoba) REFERENCES osoba (id_osoba) ON DELETE CASCADE;

ALTER TABLE zamestnanec ADD CONSTRAINT fk_zamestnanec_osoba FOREIGN KEY (id_osoba) REFERENCES osoba (id_osoba) ON DELETE CASCADE;
ALTER TABLE zamestnanec ADD CONSTRAINT fk_zamestnanec_firma FOREIGN KEY (id_firma) REFERENCES firma (id_firma) ON DELETE CASCADE;



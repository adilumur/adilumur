-- ### id vsech osob, ktere jsou vlastnici a ktere použil sluzby "Zreality" 21.02.2020.

-- smlouva_firma_vlastnik(datum_podepsani = '2020-06-08')[id_osoba]

-- SELECT DISTINCT id_osoba
-- FROM SMLOUVA_FIRMA_VLASTNIK
-- WHERE datum_podepsani = '2020-06-08';
select datum_podepsani from smlouva_najemni now();



-- ### Byty, ktere nikdo si nepronajal.

-- {smlouva_najemni[id_byt] !*> byt [id_byt]} *> byt

SELECT DISTINCT *
FROM (
    SELECT DISTINCT id_byt
    FROM BYT
    EXCEPT
    SELECT DISTINCT id_byt
    FROM SMLOUVA_NAJEMNI
) R1
NATURAL JOIN BYT BYT1;

select distinct *
from byt b where not exists (select * from smlouva_najemni s where b.id_byt = s.id_byt);

select byt.id_byt, id_adresa, byt.id_osoba, patro, byt.cislo_bytu
from byt full outer join smlouva_najemni on byt.id_osoba = smlouva_najemni.vlastnik_id_osoba 
where byt.id_osoba is null or smlouva_najemni.vlastnik_id_osoba is null;


-- ### Osoby, ktere pouzili sluzby "Zreality" v dobe od 21.02.2022 do 21.03.2022.

-- {smlouva_firma_vlastnik(datum_podepsani >= '2022-02-21' and datum_podepsani <= '2022-03-21')[id_osoba] 
-- ∪
-- smlouva_firma_najemce(datum_podepsani >= '2022-02-21' and datum_podepsani <= '2022-03-21')[id_osoba]} * osoba

SELECT DISTINCT *
FROM (
    SELECT DISTINCT id_osoba
    FROM SMLOUVA_FIRMA_VLASTNIK
    WHERE datum_podepsani >= '2022-02-21' AND datum_podepsani <= '2022-03-21'
    UNION
    SELECT DISTINCT id_osoba
    FROM SMLOUVA_FIRMA_NAJEMCE
    WHERE datum_podepsani >= '2022-02-21' AND datum_podepsani <= '2022-03-21'
) R1
NATURAL JOIN OSOBA;


-- ### Vlastnici(jmeno, prijmeni, osobni cislo, id_osoba), ktere maji byty pouze ve meste 'New-York'.

-- {adresa(mesto = 'New-York')[id_adresa] *> byt[id_adresa, id_osoba]} *> vlastnik *> osoba[jmeno, prijmeni, osobni_cislo, id_osoba]

SELECT DISTINCT jmeno,
                prijmeni,
                osobni_cislo,
                id_osoba
FROM (
    SELECT DISTINCT jmeno,
                    prijmeni,
                    osobni_cislo,
                    id_osoba
    FROM OSOBA
) R4
NATURAL JOIN (
    SELECT DISTINCT id_osoba
    FROM VLASTNIK
    NATURAL JOIN (
        SELECT DISTINCT id_adresa,
                        id_osoba
        FROM (
            SELECT DISTINCT id_adresa,
                            id_osoba
            FROM BYT
        ) R1
        NATURAL JOIN (
            SELECT DISTINCT id_adresa
            FROM ADRESA
            WHERE mesto = 'New-York'
        ) R2
    ) R3
) R5;



-- ### Data placeni za byt, id ktereho je 83.

-- {platba[datum_placeni, id_smlouva_najemni] <* smlouva_najemni(id_byt = '83')}
-- *>
-- platba[datum_placeni]

SELECT DISTINCT datum_placeni
FROM (
    SELECT DISTINCT datum_placeni
    FROM PLATBA PLATBA1
) R3
NATURAL JOIN (
    SELECT DISTINCT datum_placeni,
                    id_smlouva_najemni
    FROM (
        SELECT DISTINCT datum_placeni,
                        id_smlouva_najemni
        FROM PLATBA
    ) R1
    NATURAL JOIN (
        SELECT DISTINCT *
        FROM SMLOUVA_NAJEMNI
        WHERE id_byt = '83'
    ) R2
) R4;




-- ### Byt, najemce ktereho ma osobni cislo 156.

-- byt <* smlouva_najemni(id_osoba = '156')[id_byt]

SELECT DISTINCT id_byt,
                id_adresa,
                id_osoba,
                patro,
                cislo_bytu
FROM BYT
NATURAL JOIN (
    SELECT DISTINCT id_byt
    FROM SMLOUVA_NAJEMNI
    WHERE id_osoba = '156'
) R1;





-- ### Datum podepsani najemni smlouvy bytu id = 51.

-- smlouva_najemni(id_byt = '51')[datum_podepsani]

select distinct datum_podepsani
from smlouva_najemni where id_byt = '51';


-- ### Vypiste jmena vlastniku bytu, byty kterych jsou v meste "Praha"

select distinct jmeno 
from osoba join byt on osoba.id_osoba = byt.id_osoba join adresa on byt.id_adresa = adresa.id_adresa
where mesto = 'Praha';




-- ### id_bytu, ktery pronajma vlastnik s jmenem "Fabio"

select distinct byt.id_byt
from byt join osoba on byt.id_osoba = osoba.id_osoba join smlouva_najemni s on s.id_byt = byt.id_byt
where jmeno = 'Fabio';

-- ### Vypiste jmena a prijmeni vsech zamestnancu, ktere pracujou ve vsech firmach

select jmeno, prijmeni
from osoba join zamestnanec z on z.id_osoba = osoba.id_osoba;



-- ### Vypiste adresu(ulice a cislo domu) vsech bytu a zaroven patro

select ulice, cislo_domu, patro
from adresa join byt on adresa.id_adresa = byt.id_adresa

-- ### Vypiste id_smlouva_firma_vlastnik, ktera patri vlastnikovi s osobnim cislem 8.

-- {vlastnik(id_osoba = 8) * smlouva_firma_vlastnik}[id_smlouva_firma_vlastnik]

select id_smlouva_firma_vlastnik
from vlastnik inner join smlouva_firma_vlastnik using(id_osoba)
where id_osoba = 8



-- ### Vypiste id_smlouva_firma_najemce, ktera patri najemcovi, ktery ma datum_narozeni "22.02.2002" 


-- ### Vypiste id_bytu, ktere jsou pronajaty.

-- byt[id_byt] ∩ smlouva_najemni[id_byt]

SELECT DISTINCT id_byt
FROM BYT
INTERSECT
SELECT DISTINCT id_byt
FROM SMLOUVA_NAJEMNI;


-- ### smazte osob, ktere jsou vlastnici.

begin;
select count(id_osoba) from vlastnik;
delete from osoba where id_osoba in (select id_osoba from vlastnik);
select count(id_osoba) from vlastnik;
rollback;


-- ### Proste pro zajimovost, zvetsime patro na kterem se nachazi byty o 10.

begin;
select patro from byt;
update byt
set patro = patro + 10
where patro in (select patro from byt);
select patro from byt;
rollback;




-- ### Vyberte jmeno, prijmeni vsech osob, ktere podepsali smouva_firma_vlastnik, take vypeste id_smlouva_firma_vlastnik, vystup seradte podle prijmeni.

select o.jmeno, o.prijmeni, s.id_smlouva_firma_vlastnik
from vlastnik v full outer join smlouva_firma_vlastnik s on v.id_osoba = s.id_osoba
    left join osoba o on v.id_osoba = o.id_osoba
group by o.jmeno, o.prijmeni, s.id_smlouva_firma_vlastnik
order by o.jmeno;




-- ### Vlastnik(id_osoba) ktery uzavril smlouvu s kazdou firmou.

-- smlouva_firma_vlastnik[id_osoba, id_firma]÷firma[id_firma]

select id_osoba from vlastnik where not exists(
    select * from firma where not exists(
        select * from smlouva_firma_vlastnik where smlouva_firma_vlastnik.id_osoba = vlastnik.id_osoba and smlouva_firma_vlastnik.id_firma = firma.id_firma
    )
);
select id_osoba from vlastnik where 
    (select count(distinct id_firma) from smlouva_firma_vlastnik where smlouva_firma_vlastnik.id_osoba= vlastnik.id_osoba)
    =
    (select count(id_firma) from firma)
;

select * from firma
except 
select * from firma where id_firma in(
    select id_firma from smlouva_firma_vlastnik where firma.id_firma = smlouva_firma_vlastnik.id_firma and id_osoba in(
        select id_osoba from (
            select * from vlastnik where
            (select count(distinct id_firma) from smlouva_firma_vlastnik where smlouva_firma_vlastnik.id_osoba = vlastnik.id_osoba) 
            =
            (select count(id_firma) from firma)
        ) vlastnik
    )
);
select id_osoba, count(distinct smlouva_firma_vlastnik.id_osoba) from smlouva_firma_vlastnik where id_osoba in(
    select id_osoba from (
            select * from vlastnik where
            (select count(distinct id_firma) from smlouva_firma_vlastnik where smlouva_firma_vlastnik.id_osoba = vlastnik.id_osoba) 
            =
            (select count(id_firma) from firma)
        ) vlastnik
    )
group by id_osoba 
having count(distinct smlouva_firma_vlastnik.id_firma) != (select count(id_firma) from firma);






-- ### vyberte osoby, ktere uzavreli smlouva_firma_najemce

select *
from osoba o join smlouva_firma_najemce s on o.id_osoba = s.id_osoba




-- ### Vyberte(jmeno, prijmeni,  datum_podepsani),  datum podepsani smlouva_firma_vlastnik musi byt nejstarsi.

select o.jmeno, o.prijmeni, (select min(s.datum_podepsani) as nejstarsi_datum_podepsani from smlouva_firma_vlastnik s where s.id_osoba = v.id_osoba)
from vlastnik v join osoba o on o.id_osoba = v.id_osoba;


-- ### Vytvorte tabulku,  ve ktere budou pouze byty ktere drazsi nez 20 000Kc a vsichni ktere jsou ve USD.

CREATE VIEW nejdrazsi_byty_2 AS
SELECT id_byt as id_nejdrazsi_byt
FROM (
    select id_smlouva_najemni
    from platba where mnozstvi >= '20 000Kc'
    intersect
    select id_smlouva_najemni
    from smlouva_najemni
) r1 natural join smlouva_najemni


-- ### Vypiste id_osoba vsech vlastniku

-- vlastnik[id_osoba]

select *
from vlastnik v where exists (select * from byt b where v.id_osoba = b.id_osoba);


-- ### Vlozte nahodne data v tabulku firma.

begin;
select count(*) from firma;
insert into firma (id_firma, nazev)
    select id_firma, nazev from(
        select round(random()*1000)+1 as id_firma, 'Zreality.country' as nazev
        from firma
    )vstup 
order by random() limit 5;
select count(*) from firma;
rollback;
select count(*) from firma;





-- ### Vypiste vsechny smlouvy(id_smlouva_firma_vlastnik), ktere uzavrela firma id = 1, se vlastnikem alespon 5 krat 

select id_osoba
from firma f join smlouva_firma_vlastnik on f.id_firma = smlouva_firma_vlastnik.id_firma join vlastnik using (id_osoba)
where f.id_firma = '1' 
group by id_osoba
having count(id_osoba) >= 5
order by id_osoba desc;


-- ### vypiste kolik jsou zamestnancu celkem ve vsech firmech 

select count(*)
from zamestnanec




-- ### Vypiste ktery zamestnanec pracuje ve ktere firme^ ve vysledky musi byt jmeno, prijmeni a nazev(firma)

select jmeno, prijmeni, nazev as firma
from zamestnanec join firma on firma.id_firma = zamestnanec.id_firma join osoba on osoba.id_osoba = zamestnanec.id_osoba
group by firma, jmeno, prijmeni
order by firma;


-- ### Kolik by bylo smluv(smlouva_firma_vlastnik) pokud by kazdy vlastnik uzavril smlouvu s kazdou firmou, napriklad kdyby kazdy vlastnik mel jeden byt s kazde zeme

select count(*) from vlastnik cross join firma








































































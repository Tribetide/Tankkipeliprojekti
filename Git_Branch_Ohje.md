# Git-branchien käyttö ja hallinta
---

## 🔹 Miten kehittää eri osia erikseen?

Branchien käyttö antaa mahdollisuuden työskennellä itsenäisesti eri osa-alueiden parissa ja yhdistää valmiit ominaisuudet vasta niiden valmistuttua. Näin päähaara pysyy vakaana.

---

## 🔹 Jos haluat tehdä muutoksia tankkiin

Seuraa näitä ohjeita, kun kehität `Tank.cpp` ja `Tank.hpp` -tiedostoja erillisessä branchissa.

1️⃣ **Siirry tankkia käsittelevään branchiin:**

```sh
git checkout tank
```

2️⃣ **Tee tarvittavat muutokset** `Tank.cpp` ja `Tank.hpp` -tiedostoihin.

3️⃣ **Lisää ja commitoi muutokset:**

```sh
git add .
git commit -m "Parannettu tankin liikettä ja ampumista"
```

4️⃣ **Pushaa muutokset GitHubiin:**

```sh
git push origin tank
```

---

## 🔹 Kun branchin ominaisuus on valmis

Kun ominaisuus on testattu ja valmis yhdistettäväksi päähaaraan (`main`), toimi näin:

1️⃣ **Siirry päähaaraan:**

```sh
git checkout main
```

2️⃣ **Vedä uusin versio (jos muut ovat päivittäneet ****\`\`****-haaraa):**

```sh
git pull origin main
```

3️⃣ **Yhdistä kehitysbranch päähaaraan:**

```sh
git merge tank
```

4️⃣ **Poista branch, jos sitä ei enää tarvita:**

```sh
git branch -d tank
git push origin --delete tank
```

---

## 🔹 Hyödyllisiä lisäkomentoja

- **Listaa kaikki branchit:**
  ```sh
  git branch
  ```
- **Vaihda branchia:**
  ```sh
  git checkout branch_nimi
  ```
- **Luo uusi branch ja siirry siihen:**
  ```sh
  git checkout -b uusi_branch
  ```
- **Päivitä branch, jos ****\`\`****-haaraan on tullut muutoksia:**
  ```sh
  git merge main
  ```

---

## 🔹 Yhteenveto

Branchit tekevät kehityksestä järjestelmällisempää ja mahdollistavat erilaisten ominaisuuksien työstämisen ilman, että päähaara rikkoutuu.


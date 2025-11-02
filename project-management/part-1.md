# Koktajlownik - znajdź swój koktajl

## Nazwa projektu

**Koktajlownik** - nazwa wpisująca się w konwencję nazewnictwa w KN Solvro jednoznacznie wskazująca na rzecz wokół której budowane jest rozwiązanie... Koktajl.

## Cel projektu

Cel projektu jest prosty: Zmniejszenie czasu oczekiwania w kolejce na napoje w kąciku koktajlowym.

## Opis problemu

Studenci mający problem z wyborem swojego trunku stoją niezdecydowani w kolejce i gdy przychodzi ich pora dopiero myślą nad wyborem lub nie zdążyli wybrać. Mnogość pozycji w menu ich przytłacza i nie są w stanie od razu zamówić. Wynikać to może z paru problemów:

- Brak znajomości oferty
- Trudności z podejmowaniem decyzji

## Proponowane rozwiązanie

Można by było wdrożyć **aplikację typu PWA do przygotowywania/planowania zamówienia.** Dlaczego nie zrobić tego co wszystkie fast-foody - zamawiania online?

Tu alkohol wchodzi w grę, należałoby wprowadzić albo KYC albo inną formę weryfikacji wieku. Również trzeba by było wprowadzić stanowisko do wydawania drinków. Istnieje ryzyko kradzieży zamówień. Należy więc znaleźć inne rozwiązanie. Ostatnia kwestia, to kwestie społeczne. Barman to "twój przyjaciel", nie eliminujmy przyjemnej atmosfery i wymuszonego small-talku.

Student więc nie zamawiałby przez aplikację - **tworzyłby tylko koszyk**. Po wybraniu wszystkiego co chce **stworzy się kod QR który trzeba zaprezentować barmanowi**. Ten go skanuje i w swoim systemie PoS będzie miał wszystkie dane zamówienia: co?, ile?, oraz specjalne życzenia.

To rozwiąże pierwszy problem, braku znajomości oferty. Student przed wejściem w kolejkę, lub będąc w niej będzie w stanie zaznajomić się z ofertą i wybrać z niej to co chce. Barman od razu będzie wiedział co ma przygotować, co usprawni proces. A co jeśli mimo wszystko nie będzie potrafił się zdecydować?

Pomóżmy mu z wyborem!!! W naszym rozwiązaniu mogą być  takie sposoby na pomoc:

- Chybił trafił - losowy wybór (dla hazardzistów)
- Quiz - po którego wypełnieniu student dostanie propozycję (Oparty o ML i LLM)
- Turniej koktajli - niech student zadecyduje który jest najlepszy, może mu to pomoże z wyborem
- Chatbot proponujący drinki - jak już studenci są uzależnieni od AI, to dajmy im tę przyjemność.

Tym samym odciążamy klienta i de facto wybieramy za niego - jeśli ten jest BAAAAAARDZO niezdecydowany.

Wprowadzenie aplikacji w przedsięwbzięcie pozwoliłoby nam na stworzenie programu lojalnościowego - jest to jednak poza problemem, który próbujemy rozwiązać.

Ostatnia kwestia, dlaczego PWA?
Ludzie słusznie są sceptyczni do instalowania aplikacji na swoich telefonach. PWA to eliminuje, a mimo wszystko pozwala na dodanie skrótu na ekran użytkownika.

## Kluczowe funkcjonalności (MVP)

- Prezentacja oferty (menu w aplikacji)
- Możliwość zaplanowania zamówienia (generowanie kodu QR z "koszykiem")
- Integracja z PoS (Point of Sale) / sposób prezentacji zamówienia dla obsługi na podstawie kodu QR

## Grupa docelowa

Grupą docelową byliby klienci (stali jak i nowi) posiadający umiejętności cyfrowe i urządzenie mobilne . Możemy więc celować we wszystkich studentów PWr chętnych skorzystać z oferty Kącika Koktajlowego.

## Harmonogram

1. Analiza i planowanie
    - Definicja wymagań [1 tydzień]
    - Zespół Mobile/Frontend: - razem [1 tydzień]
        - Projekt UI/UX
        - Architektura aplikacji PWA
    - Zespół Backend: [1 tydzień]
        - Architektura części backend, modelowanie bazy danych
    - Zespół "C": [1 tydzień]
        - Analiza wykonywalności integracji z instniejącym PoS
2. Development
    - Zespół Mobile/Frontend:
        - Prezentacja menu i tworzenie koszyka [3 tygodnie]
        - Generowanie kodu QR, logika koszyka, integracja z backendem [2 tygodnie]
    - Zespół Backend:
        - Stworzenie endpointów, logiki koszyka [3 tygodnie]
    - Implementacja PoS w system (wyciąganie danych o koszyków z backendu i wprowadzanie ich do kasy) [3 tygodnie]
3. Testowanie i wdrożenie [1 tydzień]

RAZEM: 11 tygodni

## Ryzyka i sposoby ich ograniczenia

1. Niechętność korzystania z rozwiązania - Klienci mogą nie wiedzieć o istnieniu aplikacji lub będą woleć kultywowanie swoich dawnych nawyków stania w kolejce
    - Jak to ograniczyć?
        - Zróbmy kampanię promocyjną promującą aplikację, dająć np. rabat na zamówienie - pieniądze przemawiają do studentów.
        - Jasna komunikacja - Zamów szybciej, zeskanuj kod.
2. Osoby niezdecydowane w kolejce, niekorzystające z aplikacji mogą tworzyć zator na który aplikacja nie będzie miała wpływu
    - Jak to ograniczyć?
        - Stworzyć oddzielną kolejkę dla osób korzystających z aplikacji (Fast lane)
3. Trudności techniczne z implementacją rozwiązania z istniejącym systemem Point of Sale (PoS)
    - Jak to ograniczyć?
        - Gdy nastąpią stworzyć alternatywną formę prezentacji koszyka dla barmana - "nabijanie" na kasę musiałoby się odbywać w sposób manualny
4. Brak zrozumienia przez klienta, że po złożeniu koszyka należy podejść do barmana
    - Jak to ograniczyć?
        - Jasne instrukcje na każdym etapie tworzenia koszyka w aplikacji.

## Jak zmierzyć sukces projektu?

Zadaniem tego projektu jest zmniejszyć czas oczekiwania w kolejce... Więc powinniśmy być w stanie sprawdzić czy ma to miejsce. Robiąc eksperyment możemy zmierzyć średni czas, w którym obsługujemy klienta przed i po wprowadzeniu naszego rozwiązania. Jeśli ten się zmniejszy, to projekt odniósł sukces.

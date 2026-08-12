Con questo scatter file completo, ecco la situazione reale:

**Buona notizia**: `0x24000000–0x2407FFFF` (AXI SRAM, dove va tutto il tuo `.ANY (+RW +ZI)`) è un bus/dominio di memoria completamente diverso da `0x30004000` (SRAM1, dominio D2). Quindi il linker **non ha alcuna region che possa "scivolare" per errore** su SRAM1 — il tuo heap lwIP a `0x30004000` non viene toccato per il semplice fatto che nessuna regola generica lo copre.

**Ma non è protetto esplicitamente**

Guarda bene: hai riservato esplicitamente `0x30040000` (SRAM3, per descrittori DMA + pool RX), ma **non hai una region dedicata per `0x30004000`** (SRAM1, dove punta `LWIP_RAM_HEAP_POINTER`). Oggi funziona per "assenza di conflitto", non per progetto — se in futuro tu o un collega aggiungete un'altra region con matching più ampio, o un middleware che si aspetta di poter usare SRAM1 liberamente, quell'indirizzo non è protetto da nulla.

**Consiglio: aggiungi la region mancante, coerente con lo stile che usi già**

```
; ram for lwip heap - SRAM1 @ 0x30004000
  RW_LWIP_HEAP 0x30004000 EMPTY 0x3800  {
  }
```

`0x3800` = 14*1024 = il tuo `MEM_SIZE` esatto. Mettila come region indipendente nel file, allo stesso livello di `RW_DMARxDscrTab` ecc. — non dentro `LR_IROM1`, perché è un'altra area di RAM (SRAM1 vs AXI), quindi va aggiunta come ulteriore load region a sé stante, esattamente come hai già fatto per le tre region SRAM3.

**Un'osservazione sul source della SRAM1 su H7**

SRAM1 e SRAM2 (0x30000000–0x30040000) sono nel dominio D2, come SRAM3 — quindi coerenti con dove hai già messo Ethernet DMA e pool RX. Ha senso tenere lì anche l'heap lwIP (serve comunque DMA coerenza per i buffer di rete). Ricontrolla solo che nel tuo `MPU_Config` (se lo usi) l'intera regione `0x30000000–0x30048000` sia configurata come **Normal, non-cacheable** (o cacheable con gestione esplicita del cache maintenance) — è un errore molto comune con l'Ethernet su H7 dimenticarsi la MPU su queste region e vedere pacchetti corrotti in modo intermittente.





Confermato dal datasheet ST: sul STM32H747xI/G, SRAM1 (dominio D2) è 128 KB, SRAM2 (dominio D2) è 128 KB, SRAM3 (dominio D2) è 32 KB, SRAM4 (dominio D3) è 64 KB, oltre a 512KB di AXI SRAM.

Il layout d'indirizzi standard su H747 è:

| Region | Range                       | Dimensione         |
| ------ | --------------------------- | ------------------ |
| SRAM1  | `0x30000000` – `0x3001FFFF` | 128 KB             |
| SRAM2  | `0x30020000` – `0x3003FFFF` | 128 KB             |
| SRAM3  | `0x30040000` – `0x30047FFF` | 32 KB              |
| SRAM4  | `0x38000000` – `0x3800FFFF` | 64 KB (dominio D3) |

Quindi:

- `0x30004000` (il tuo heap lwIP) → cade dentro **SRAM1** (offset +0x4000)
- `0x30040000` (i tuoi descrittori DMA/pool RX) → cade dentro **SRAM3**

Sono due bank fisiche diverse, entrambe nel dominio D2, ma non la stessa memoria. Questo conferma quanto detto prima: nessun overlap fisico, ma vale comunque la pena riservare esplicitamente lo spazio dell'heap in SRAM1 con una region `EMPTY` nello scatter file, visto che oggi quell'area (a differenza di SRAM3, tutta occupata dai tuoi tre blocchi Ethernet) è totalmente libera e a rischio di essere riempita dal linker con altre variabili RW/ZI.
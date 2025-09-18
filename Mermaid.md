```mermaid
flowchart TD
    K[Klavyedeki tuş] --> E[Klavyedeki Encoder]
    E -->|Scan code| C[8042 Controller]
    C -->|IRQ1| PIC[Programmable Interrupt Controller]
    PIC --> CPU
    CPU -->|IDT'de lookup| ISR[Keyboard Interrupt Handler]
    ISR --> Buffer[Keyboard buffer] --> App[Uygulama]

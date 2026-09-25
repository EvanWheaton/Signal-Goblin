import { useEffect, useState, type ElementType } from "react";
import { BatteryCharging, Cpu, Gamepad2, Radio, ScanLine, ShieldCheck } from "lucide-react";

const features: { kicker: string; title: string; copy: string; Icon: ElementType }[] = [
  { kicker: "THE BRAINS", title: "Dual-MCU architecture", copy: "ESP32-C5 N16R8 + STM32WB55CGU6 split the work so the goblin can stay responsive while it listens.", Icon: Cpu },
  { kicker: "THE FACE", title: "3.5” TFT + touch", copy: "ILI9488 over SPI, connected through a 40-pin FPC adapter. Menus, scopes, and tiny goblin animations live here.", Icon: ScanLine },
  { kicker: "THE EARS", title: "Multi-band antennas", copy: "A goblin-head PCB with antenna ears for the 2.4 GHz and sub-GHz worlds. Weird shape, deliberate RF.", Icon: Radio },
  { kicker: "THE HANDS", title: "Tactile control", copy: "A 5-way SMD joystick, boot, and reset buttons keep the important actions one click away.", Icon: Gamepad2 },
  { kicker: "THE MEMORY", title: "SD card logging", copy: "Capture snippets, packet metadata, and field notes directly to removable storage.", Icon: ShieldCheck },
  { kicker: "THE FUEL", title: "1500 mAh LiPo", copy: "USB-C in, buck converter down. Portable enough for the bench, the basement, or a very suspicious walk.", Icon: BatteryCharging },
];

const menuItems = ["RF CATCHER", "PACKET SNIFFER", "NFC TOOLS", "IR PLAYGROUND", "GAMES"];

export default function Features() {
  const [selected, setSelected] = useState(0);
  useEffect(() => {
    const timer = window.setInterval(() => setSelected((value) => (value + 1) % menuItems.length), 1800);
    return () => window.clearInterval(timer);
  }, []);

  return <div className="page"><section className="page-hero"><div className="container"><div className="eyebrow"><span className="pulse-dot" /> SYSTEM MAP / 06 MODULES</div><h1>Built for the<br /><em>invisible layer.</em></h1><p>Not a single-purpose gadget. A pocket-sized playground for protocols, packets, and the spaces between them.</p></div></section>
    <section className="section goblin-menu-section"><div className="container goblin-menu-grid"><div><div className="section-kicker">/ FIRMWARE / MENU 01</div><h2>A little<br /><span className="text-green">mischief</span> on screen.</h2><p className="muted-copy">The custom firmware turns signal hunting into a tactile arcade: pixel goblin animations, quick-launch tools, and a menu that feels alive while the hardware does the serious work.</p><div className="menu-chip-row"><span>TOUCH UI</span><span>5-WAY JOYSTICK</span><span>PIXEL ANIMATION</span></div></div><div className="goblin-console" aria-label="Animated pixel goblin firmware menu"><div className="console-scanlines" /><div className="console-top"><span>SIGNAL GOBLIN // FW 0.1</span><span>● REC</span></div><div className="console-screen"><div className="pixel-goblin-frame"><img src="/manus-storage/IMG_0191_0a135d6a.PNG" alt="Signal Goblin prototype" /><div className="pixel-goblin-glitch" /></div><div className="console-menu">{menuItems.map((item, index) => <button key={item} className={selected === index ? "console-item selected" : "console-item"} onClick={() => setSelected(index)}><span>{selected === index ? "▶" : "·"}</span>{item}</button>)}</div></div><div className="console-bottom"><span>↑↓ SELECT</span><span>A OPEN</span><span>B BACK</span></div></div></div></section>
    <section className="section"><div className="container"><div className="feature-grid">{features.map(({ kicker, title, copy, Icon }) => <article className="feature-card" key={title}><div className="feature-icon"><Icon size={21} /></div><div className="section-kicker">/ {kicker}</div><h3>{title}</h3><p>{copy}</p></article>)}</div></div></section>
    <section className="section spec-section"><div className="container spec-grid"><div><div className="section-kicker">/ QUICK SPECS</div><h2>Compact by<br /><span className="text-khaki">design.</span></h2></div><div className="spec-list"><div><span>Display</span><strong>3.5” ILI9488 / SPI / touch</strong></div><div><span>Power</span><strong>1500 mAh LiPo / USB-C</strong></div><div><span>Storage</span><strong>Micro SD / FAT32</strong></div><div><span>Form factor</span><strong>Goblin-head PCB / antenna ears</strong></div></div></div></section></div>;
}

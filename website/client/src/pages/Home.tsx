import { useEffect, useState } from "react";
import { Link } from "wouter";
import { ArrowRight, Radio, ChevronRight } from "lucide-react";

const modules = ["CC1101 / 433 MHz", "PN532 / NFC", "IR TX + RX", "nRF24 / 2.4 GHz", "SD logging", "5-way joystick"];
const typedLines = ["Make noise.", "Catch signals."];

export default function Home() {
  const [line, setLine] = useState(0);
  const [visible, setVisible] = useState(0);

  useEffect(() => {
    const current = typedLines[line];
    if (visible < current.length) {
      const timer = window.setTimeout(() => setVisible((value) => value + 1), 72);
      return () => window.clearTimeout(timer);
    }
    const pause = window.setTimeout(() => {
      setVisible(0);
      setLine((value) => (value + 1) % typedLines.length);
    }, 2200);
    return () => window.clearTimeout(pause);
  }, [line, visible]);

  return <>
    <section className="hero hero-image"><div className="hero-overlay" /><div className="container hero-content">
      <div className="eyebrow"><span className="pulse-dot" /> HARDWARE LAB / SIGNAL-GOBLIN-01</div>
      <h1 className="terminal-heading"><span className="terminal-prompt">&gt;_</span> {typedLines[line].slice(0, visible)}<span className="typing-cursor">▋</span><br /><em>{line === 0 ? "Catch signals." : "Make noise."}</em></h1>
      <p className="hero-lede">A goblin-headed, dual-MCU pocket lab for exploring the invisible. Built from scratch, tuned by hand, and made to be opened.</p>
      <div className="hero-actions"><Link href="/features" className="button button-primary">Explore the rig <ArrowRight size={16} /></Link><Link href="/build-log" className="button button-ghost">Read the build log <span>↗</span></Link></div>
      <div className="hero-readout"><span>BAT 4.18V</span><span>FW 0.1.0-alpha</span><span>RF / NFC / IR / 2.4</span></div>
    </div><div className="hero-caption">Prototype render / visual direction<br /><span>01 — THE GOBLIN HEAD</span></div></section>
    <section className="section intro-section"><div className="container split-grid"><div><div className="section-kicker">/ 001 — THE IDEA</div><h2>Curiosity needs<br /><span className="text-green">a tool.</span></h2></div><div className="intro-copy"><p>Signal Goblin is a custom pentesting tool in the middle of its first PCB prototype. Two brains, one mischievous shell, and a firmware layer that treats radio capture like an arcade game.</p><Link href="/build-log" className="text-link">Follow the build <ChevronRight size={16} /></Link></div></div></section>
    <section className="section module-section"><div className="container"><div className="section-kicker">/ 002 — THE LOADOUT</div><div className="section-head"><h2>Small board.<br /><span className="text-khaki">Big appetite.</span></h2><p>Every module has a job. Every pin has a story.</p></div><div className="module-grid">{modules.map((module, i) => <div className="module-card" key={module}><span className="module-number">0{i + 1}</span><span>{module}</span><span className="module-arrow">↗</span></div>)}</div></div></section>
    <section className="signal-strip"><div className="container signal-strip-inner"><span className="signal-icon"><Radio size={19} /></span><span>SCANNING THE AIR BETWEEN THINGS</span><span className="signal-line" /><span className="signal-value">433.920 MHz</span></div></section>
    <section className="section architecture-section"><div className="container"><div className="section-kicker">/ 003 — UNDER THE SILK</div><div className="architecture-grid"><div><h2>Two brains.<br /><span className="text-green">One goblin.</span></h2><p className="muted-copy">The ESP32-C5 handles the high-speed mischief. The STM32WB55 keeps the low-power, low-level work tidy. A shared display, a shared bus, and a shared appetite for packets.</p></div><div className="chip-stack"><div className="chip-card"><span className="chip-tag">MCU / 01</span><strong>ESP32-C5</strong><span>Wi-Fi 6 · 5 GHz · UI</span></div><div className="chip-connector">⇄ shared SPI / UART</div><div className="chip-card chip-card-khaki"><span className="chip-tag">MCU / 02</span><strong>STM32WB55</strong><span>BLE · Sub-GHz · power</span></div></div></div></div></section>
    <section className="cta-section"><div className="container cta-inner"><div><div className="section-kicker">/ READY TO TINKER?</div><h2>Come see what<br /><span className="text-green">the goblin catches.</span></h2></div><Link href="/downloads" className="button button-primary">Get the files <ArrowRight size={16} /></Link></div></section>
  </>;
}

import type { ReactNode } from "react";
import { useState } from "react";
import { Link, useLocation } from "wouter";
import { Menu, X, Github, Radio, ArrowUpRight } from "lucide-react";

const navItems = [
  { href: "/features", label: "Features" },
  { href: "/build-log", label: "Build log" },
  { href: "/downloads", label: "Downloads" },
];

export default function SiteShell({ children }: { children: ReactNode }) {
  const [location] = useLocation();
  const [open, setOpen] = useState(false);
  return (
    <div className="site-shell">
      <header className="site-nav">
        <div className="nav-inner">
          <Link href="/" className="brand" onClick={() => setOpen(false)}>
            <span className="brand-mark"><Radio size={15} strokeWidth={2.5} /></span>
            <span>signal<span className="brand-green">/</span>goblin</span>
          </Link>
          <nav className={open ? "nav-links nav-links-open" : "nav-links"}>
            {navItems.map((item) => (
              <Link key={item.href} href={item.href} onClick={() => setOpen(false)} className={location === item.href ? "nav-link active" : "nav-link"}>{item.label}</Link>
            ))}
            <a className="nav-link nav-github" href="https://github.com" target="_blank" rel="noreferrer"><Github size={14} /> GitHub <ArrowUpRight size={13} /></a>
          </nav>
          <button className="mobile-menu" aria-label="Toggle navigation" onClick={() => setOpen(!open)}>{open ? <X size={19} /> : <Menu size={19} />}</button>
        </div>
      </header>
      <main>{children}</main>
      <footer className="site-footer">
        <div className="container footer-grid">
          <div><div className="footer-brand">signal/goblin<span className="brand-green">_</span></div><p>Open hardware for curious hands.</p></div>
          <div className="footer-meta"><span>PCB prototype / rev 0.1</span><span className="status-dot">● &nbsp;designing in public</span></div>
        </div>
      </footer>
    </div>
  );
}

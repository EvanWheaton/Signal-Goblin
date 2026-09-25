# Custom domain setup

The site currently uses the placeholder domain `your-domain.example` in its canonical URL, Open Graph metadata, Twitter metadata, robots file, and sitemap. Replace it with the final domain before production launch.

## Files to update

- `client/index.html`: replace every `https://your-domain.example` URL and update the `og:image` path when the final social preview image is available.
- `client/public/robots.txt`: replace the sitemap URL.
- `client/public/sitemap.xml`: replace the domain in each `<loc>` entry.

## DNS checklist

Create the DNS records required by the chosen hosting provider. Common setups use an `A` or `ALIAS` record for the apex domain and a `CNAME` record for `www`. Enable HTTPS at the hosting provider, then verify the canonical URL, `robots.txt`, `sitemap.xml`, and social preview metadata.

No DNS records or hosting settings have been changed by this commit.

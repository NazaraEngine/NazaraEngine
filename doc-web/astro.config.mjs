import {defineConfig} from 'astro/config';
import starlight from '@astrojs/starlight';
import tailwind from '@astrojs/tailwind';

// https://astro.build/config
export default defineConfig({
    site: 'https://sirmishaa.github.io/NazaraEngine',
    base: '/NazaraEngine/',
    integrations: [
        starlight({
            title: 'Nazara documentation',
            social: {
                github: 'https://github.com/NazaraEngine/NazaraEngine',
            },
            sidebar: [
                {
                    label: 'Guides',
                    items: [
                        // Each item here is one entry in the navigation menu.
                        {label: 'Example Guide', slug: 'guides/example'},
                    ],
                },
                {
                    label: 'Reference',
                    collapsed: true,
                    autogenerate: {directory: 'reference'},
                },
            ],
            customCss: ['./src/tailwind.css'],
        }),
        tailwind({applyBaseStyles: false}),
    ],
});

'use strict';

const version = "0.5.0";
const cacheName = `wordament-solver-${version}`;
self.addEventListener('install', e => {
  e.waitUntil(
    caches.open(cacheName).then(cache => {
      return cache.addAll([
        './',
        './src/index.js',
        './src/stylesheet.css',
        './src/wordament.js',
        './src/wordament.js.map',
        './src/wordament.wasm',
        './assets/wordlist.txt',
      ])
          .then(() => self.skipWaiting());
    })
  );
});

self.addEventListener('activate', event => {
  event.waitUntil(self.clients.claim());
});

self.addEventListener('fetch', event => {
  event.respondWith(
    caches.open(cacheName)
      .then(cache => cache.match(event.request, {ignoreSearch: true}))
      .then(response => {
      return response || fetch(event.request);
    })
  );
});

'use strict';

(function(){
  if ('serviceWorker' in navigator) {
    window.addEventListener('load', async function() {
       navigator.serviceWorker.register('../sw.js').then(function(reg) {
         console.log('Service worker registration success', reg);
       }).catch(function() {
         return console.error.apply(this, ['Service worker registration failed'].concat(Array.from(arguments)));
       });
    })
  } else {
    console.log('Service worker not supported');
  }
})();

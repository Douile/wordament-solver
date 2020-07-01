/*******************************************************************************
*** Classes
*******************************************************************************/

class Wordlist {
  constructor(pointer) {
    this.pointer = pointer;
  }
  [Symbol.iterator]() {
    const readWord = Module.cwrap('wordlist_readword', 'string', ['number']);
    const readPoints = Module.cwrap('wordlist_readpoints', 'number', ['number']);
    const readStack = Module.cwrap('wordlist_readstack', 'number', ['number'])
    const nextWord = Module.cwrap('wordlist_nextword', 'number', ['number']);
    let node = this.pointer;
    let i = 0;
    return {
      next: function() {
        node = nextWord(node);
        if (node === 0) return { value: i, done: true };
        i++;
        return { value: {
          word: readWord(node),
          points: readPoints(node),
          stack: readStack(node)
        }, done: false };
      }
    }
  }
  toArray() {
    return Array.from(this);
  }
  log() {
    for (let word of this) {
      console.log(word);
    }
  }
}

class InitializeWaiter {
  constructor() {
    if ('Module' in window) {
      Module['onRuntimeInitialized'] = this.initialized.bind(this);
    } else {
      const i = this.initialized.bind(this);
      Object.defineProperty(window, 'Module', {
        configurable: true,
        get: function() {return undefined;},
        set: function(value) {
          value['onRuntimeInitialized'] = i;
          Object.defineProperty(window, 'Module', { value: value });
        }
      })
    }

    this.ready = false;
    this._q = [];
  }
  initialized() {
    console.log('Runtime ready');
    this.ready = true;
    const ql = this._q.length;
    for (let i=0;i<ql;i++) {this._q[i]()};
  }
  isReady() {
    return this.ready;
  }
  wait() {
    if (this.ready) return Promise.resolve();
    return new Promise((resolve) => {
      this._q.push(resolve);
    })
  }
}

class SolveQueue {
  constructor() {
    this.active = false;
    this.next = false;
  }
  solve() {
    if (this.active) {
      this.next = true;
    } else {
      this.active = true;
      solveCurrentBoard();
      this.active = false;
      if (this.next) {
        this.next = false;
        this.solve();
      }
    }
  }
}

/*******************************************************************************
*** WASM iface
*******************************************************************************/

const Wordament = Object.defineProperties({}, {
  STATES: {
    get: function() {
      return {
        WORDLIST: 1,
        BOARD: 1 << 1
      }
    }
  },
  version: {
    get: function() {
      return Module.cwrap('version', 'number', []);
    }
  },
  getState: {
    get: function() {
      return Module.cwrap('get_state', 'number', []);
    }
  },
  state: {
    get: function() {
      const s = {};
      const _s = this.getState();
      for (let key in this.STATES) {
        s[key] = (_s & this.STATES[key]) !== 0;
      }
      return s;
    }
  },
  loadWordlist: {
    get: function() {
      return Module.cwrap('load_wordlist', 'number', ['string']);
    }
  },
  printWordlist: {
    get: function() {
      return Module.cwrap('print_wordlist', 'number', ['number']);
    }
  },
  loadBoard: {
    get: function() {
      return Module.cwrap('load_board', 'number', ['string']);
    }
  },
  search: {
    get: function() {
      return function() {
        const wordlist = Module.cwrap('search', 'number', [])();
        return new Wordlist(wordlist);
      }
    }
  },
  setDebug: {
    get: function() {
      return Module.cwrap('set_debug', null, ['number']);
    }
  },
  readStack: {
    writable: false,
    value: function(ptr) {
      const POSITIONS = 16;
      let stack = new Uint32Array(POSITIONS);
      let pos;
      for (let i=0;i<POSITIONS && pos !== 0;i++) {
        pos = Module.getValue(ptr + (i*4), 'i32');
        stack[i] = pos;
      }
      return stack;
    }
  },
  initialized: {
    writable: false,
    value: new InitializeWaiter()
  },
  solveQueue: {
    writable: false,
    value: new SolveQueue()
  }
})

/*******************************************************************************
*** Functions
*******************************************************************************/

function solveCurrentBoard() {
  clearActive();
  if (!Wordament.state.WORDLIST) return;
  let boardText = '';
  for (let el of document.querySelectorAll('.input-box > input')) {
    const tile = el.value.replace(new RegExp('[^a-z\\-\\/]','gi'), '').toLowerCase();
    if (tile.length === 0) {
      return;
    }
    boardText += tile+'\n';
  }
  location.hash = btoa(boardText);
  Wordament.loadBoard(boardText);
  const output = Wordament.search();

  const outputBox = document.querySelector('.output-box');
  if (output === null) throw new Error('Unable to find output box');
  const generator = output[Symbol.iterator]();
  let word = generator.next();
  for (let child of outputBox.children) {
    if (word.done) {
      child.remove();
    } else {
      child.setAttribute('data-stack', word.value.stack);
      child.children[0].innerText = `[${word.value.points}]`;
      child.children[1].innerText = word.value.word;
      word = generator.next();
    }
  }

  while (!word.done) {
    const el = document.createElement('tr');
    el.setAttribute('data-stack', word.value.stack);
    const elPoints = document.createElement('td');
    elPoints.innerText = `[${word.value.points}]`;
    const elWord = document.createElement('td');
    elWord.innerText = word.value.word;
    el.appendChild(elPoints);
    el.appendChild(elWord);
    outputBox.appendChild(el);
    word = generator.next();
  }

  // console.log(output.toArray());
}

function clearActive() {
  let cleared = 0;
  for (let el of document.querySelectorAll('.active')) {
    el.classList.remove('active');
    cleared++;
  }
  return cleared;
}

const displayStack = {
  active: false,
  cancel: undefined,
  waitForCancel: [],
  display: async function (stack) {
    await displayStack.cancelCurrent();
    displayStack.active = true;
    if (clearActive() > 0) {
      await displayStack.delay(500);
    }
    for (let index of Wordament.readStack(parseInt(stack))) {
      if (index === 0) break;
      await displayStack.delay(250);
      document.querySelector(`.input-box > *:nth-child(${index})`).classList.add('active');
    }
    displayStack.active = false;
  },
  delay: function (time) {
    const delay = new Promise((resolve) => {setTimeout(resolve, time)});
    const cancel = new Promise((resolve, reject) => {displayStack.cancel = reject});
    return new Promise((resolve, reject) => {
      Promise.race([delay, cancel]).then(function() {
        displayStack.cancel = undefined;
        resolve.apply(this, arguments);
      }).catch(function() {
        displayStack.cancel = undefined;
        reject.apply(this, arguments);
      });
      for (let canceller of displayStack.waitForCancel) {
        canceller();
      }
    })
  },
  cancelCurrent: async function () {
    if (!displayStack.active) return;
    if (displayStack.cancel === undefined) {
      await displayStack.awaitCancel();
    }
    displayStack.cancel('Cancelling display');
  },
  awaitCancel: function () {
    return new Promise((resolve) => {
      displayStack.waitForCancel.push(resolve);
    })
  }
}


/*******************************************************************************
*** Events
*******************************************************************************/

window.addEventListener('input', function(e) {
  if (e.target.parentNode.classList.contains('input-box')) {
    return Wordament.solveQueue.solve();
  }
});

window.addEventListener('click', function(e) {
  const stack = e.target.parentNode.getAttribute('data-stack')
  if (stack !== null) {
    displayStack.display(stack).then(null, console.warn);
  }
})

/*******************************************************************************
*** Init
*******************************************************************************/

function *splitWordlist(wordlist, splitSize) {
  const words = wordlist.split('\n');
  const wl = words.length;
  let chars = 0;
  let last = 0;
  for (let i=0;i<wl;i++) {
    chars += words[i].length;
    if (chars > splitSize) {
      yield words.slice(last, i).join('\n');
      chars = words[i].length;
      last = i;
    }
  }
  yield words.slice(last, wl).join('\n');
}

function loadHash() {
  if (location.hash.length > 1) {
    let board;
    try {
      board = atob(location.hash.substring(1)).split('\n');
    } catch(e) {
      return console.error(e);
    }
    const size = board.length;
    for (let i=0;i<size;i++) {
      const el = document.querySelector(`.input-box > input:nth-child(${i+1})`);
      if (el !== null) el.value = board[i];
    }
  }
}

async function init() {
  const res = await fetch('./assets/wordlist.txt', { mode: 'same-origin', credentials: 'omit' });
  const text = await res.text();
  console.log('Wordlist downloaded...');
  await Wordament.initialized.wait();
  // Wordament.setDebug(1); // Don't turn on verbose here unless you wanna crash your browser
  for (let split of splitWordlist(text, 512)) {
    const s = Wordament.loadWordlist(split);
    if (s !== 0) throw new Error('Error parsing wordlist');
  }
  console.log('Wordlist loaded');
  document.querySelector('.loading').removeAttribute('open');
  Wordament.solveQueue.solve();
}

init().then(null, console.error);
loadHash();

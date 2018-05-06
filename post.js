let login = {
  "method":"login",
  "params": {
    "login":"wallet_address",
    "pass":"x",
    "rigid":"",
    "agent":"xmr-wasm/0.1"
  },
  "id":1
};
let login_id;

const cn_hash = Module.cwrap('hash', 'number', ['array', 'number', 'number', 'number']);
const cn_allocate_state = Module.cwrap('allocate_state');
const cn_free_state = Module.cwrap('free_state');

function init_socket() {
  const socket = new WebSocket(`ws://${location.hostname}:8081`);
  socket.addEventListener('open', (event) => {
    console.log(`WebSocket opened: ${event}`);
    socket.send(JSON.stringify(login));
  });
  socket.addEventListener('message', (event) => {
    console.log(`WebSocket message: ${event.data}`);
    let data = JSON.parse(event.data);
    if(data.error) {
      console.log(`Error: ${data.error.code} ${data.error.message}`);
      return;
    }
    if(data.result && data.result.status) {
      console.log(`Result status: ${data.result.status}`);
    }
    if(data.result && data.result.job) {
      login_id = data.result.id;
      do_work(data.result.job, socket);
    } else if(data.method && data.method == 'job' && login_id != null) {
      do_work(data.params, socket);
    }
  });
  socket.addEventListener('error', (event) => {
    console.log(`WebSocket error: ${event}`);
  });
  socket.addEventListener('close', (event) => {
    console.log(`WebSocket closed: ${event.code} ${event.reason}`);
    login_id = null;
  });
}

function do_work(job, socket) {
  console.log('Start hashing');

  cn_allocate_state();
  console.log('CN allocated state');

  let bytes = job.blob.match(/.{2}/g).map(h => parseInt(h, 16));
  let blob = new Uint8Array(bytes);

  bytes = job.target.match(/.{2}/g).map(h => parseInt(h, 16));
  let target = new Uint8Array(bytes);
  let target_high = 0;
  let target_low = 0;
  for(let i=0; i<4; i++) {
    target_low |= target[i] << (i*8);
  }
  if(target.length > 4) {
    for(let i=4; i<8; i++) {
      target_high |= target[i] << ((i-4)*8);
    }
  }
  console.log('Target (low, high):', target_low, target_high);

  const p = cn_hash(blob, blob.length, target_low, target_high);
  const resultView = new Uint8Array(Module.HEAP8.buffer, p, 36);
  const result = new Uint8Array(resultView);
  const hash_bytes = result.slice(0,32);
  let hash = '';
  for(let i=0; i<hash_bytes.length; i++) {
    hash += ('0'+hash_bytes[i].toString(16)).substr(-2);
  }
  const nonce_bytes = result.slice(32);
  let nonce = '';
  for(let i=0; i<4; i++) {
    nonce += ('0'+nonce_bytes[i].toString(16)).substr(-2);
  }
  console.log(`Result hash: ${hash}, nonce: ${nonce}`);

  cn_free_state();
  console.log('CN freed state');

  let submit = {
    "method":"submit",
    "params": {
      "id": login_id,
      "job_id": job.job_id,
      "nonce": nonce,
      "result": hash
    },
    "id":1
  };

  socket.send(JSON.stringify(submit));
}

Module.onRuntimeInitialized = () => {
  console.log('Runtime initialized');
  setTimeout(init_socket, 1000);
};


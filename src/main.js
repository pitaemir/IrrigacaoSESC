const firebaseConfig = {
    apiKey: "AIzaSyA1LI1y7m9T-av1WW1sDQMSdqyaG6RtDFU",
    authDomain: "hortasesc-9b067.firebaseapp.com",
    databaseURL: "https://hortasesc-9b067-default-rtdb.firebaseio.com",
    projectId: "hortasesc-9b067",
    storageBucket: "hortasesc-9b067.firebasestorage.app",
    messagingSenderId: "892214843424",
    appId: "1:892214843424:web:189c891942a86138e4c445"
  };

  // ---- INICIALIZA FIREBASE ----
const app = firebase.initializeApp(firebaseConfig);
const db = firebase.database();
// ---- REFERÊNCIA ----
const historicoRef = db.ref("historico_sensores");

// ---- ARRAYS DE DADOS ----
let timestamps = [];
let temperaturas = [];
let vazoes = [];

// ---- CHARTS ----
const ctxTemp = document.getElementById("graficoTemp").getContext("2d");
const ctxFlow = document.getElementById("graficoFlow").getContext("2d");

const chartTemp = new Chart(ctxTemp, {
  type: "line",
  data: {
    labels: timestamps,
    datasets: [{
      label: "Temperatura (°C)",
      data: temperaturas,
      borderColor: "#0077cc",
      fill: false,
      tension: 0.2
    }]
  },
  options: {
    responsive: true,
    scales: { y: { beginAtZero: false } }
  }
});

const chartFlow = new Chart(ctxFlow, {
  type: "line",
  data: {
    labels: timestamps,
    datasets: [{
      label: "Vazão (L/min)",
      data: vazoes,
      borderColor: "#00b37d",
      fill: false,
      tension: 0.2
    }]
  },
  options: {
    responsive: true,
    scales: { y: { beginAtZero: true } }
  }
});

// ---- LER DADOS ----
historicoRef.on("value", (snapshot) => {
  const data = snapshot.val();
  timestamps.length = 0;
  temperaturas.length = 0;
  vazoes.length = 0;

  for (let key in data) {
    const item = data[key];
    timestamps.push(item.timestamp);
    temperaturas.push(item.temperature);
    vazoes.push(item.flowRate);
  }

  chartTemp.update();
  chartFlow.update();
});
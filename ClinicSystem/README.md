# MediFlow Clinic Management System (C Backend)

A robust Clinic Management System with a pure C backend and a modern HTML5 frontend.

## 🚀 Features
- **Data Structures**: Uses AVL Trees (Interval Management), Linked Lists (Timeline), and Arrays (Auth).
- **Novelty Feature**: Warning system with Cascade Update (Extending an appointment shifts all subsequent ones automatically).
- **Collision Handling**: Suggests alternate slots using AVL tree lookup.
- **Frontend**: Premium Dark Mode Dashboard.

## 🛠️ Compilation (Windows)

You need `gcc` (MinGW) installed.

1. Open Terminal in this folder.
2. Run the build command:
   ```powershell
   gcc backend/server.c backend/clinic.c -o server -lws2_32
   ```

## ▶️ How to Run

1. Start the server:
   ```powershell
   ./server
   ```
   You should see: `Server running on port 8080...`

2. Open the Frontend:
   - Double-click `frontend/index.html` to open it in your browser.
   - Or drag the file into Chrome/Edge.

3. **Login**:
   - Initial Doctors are Pre-seeded:
     - Name: `Dr.Smith` | Pass: `1234`
     - Name: `Dr.Jones` | Pass: `5678`
   - Or click "Register New Doctor".

## 📚 Logic Overview (DSA)

- **Auth**: Linear search in Doctor Array (O(D)).
- **Daily Schedule**: Linked List sorted by Start Time.
- **Collision Detection**: AVL Tree storing Interval [Start, End]. Search is O(log N).
- **Rescheduling**: Recursive function `propagate_delay` shifts all future events in the Linked List if `End > Next.Start`.

## ⚠️ Known Constraints
- The backend serves API on `localhost:8080`.
- Ensure no other service is using port 8080.
- If you see Network Errors, check if `server.exe` is running.

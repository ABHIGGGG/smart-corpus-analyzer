# 📚 Smart Corpus Analyzer

> **Automated Corpus Analysis & Querying Tool**
> Focused on efficient querying and paragraph retrieval using **tries** and **heaps** in C++
> Combines LLM integration (Gemini API) with classic DSA

---

## ❓ Why This Project

Modern **Large Language Models (LLMs)** are powerful, but they struggle with **context limits** (i.e., token window restrictions). Feeding large documents or datasets directly into an LLM is infeasible for many applications.

**This project addresses that challenge.**

### Key Contributions:

* ⚙️ **Efficient Paragraph Retrieval**: Retrieves only the **top-k** relevant paragraphs from a large corpus using **tries** (for prefix-based matching) and **heaps** (for top-k scoring).
* 🧠 **LLM-Aided Answering**: After identifying the most relevant content, a small context is passed to **Gemini** (via API) for high-quality answers.
* 🚀 **Time-Efficient**: Entire query and retrieval pipeline runs in **O(k + n)** time, where:

  * `n` = length of the question
  * `k` = number of top paragraphs to fetch

---

## 🗂 File Structure

```
.
├── Node.cpp          # Handles paragraph node structure
├── dict.cpp          # Dictionary-based paragraph search logic
├── qna_tool.cpp      # Integration of paragraph retrieval and Gemini call
├── tester.cpp        # Main driver file – to toggle between Part 1 and Part 2
├── paragraphs.txt    # Top 5 paragraphs will be stored here
├── .env              # Your Gemini API key (for Part 2)
└── README.md         # You're reading it!
```

---

## 🔧 Setup

### 1. Install dependencies (for Part 2):

```bash
pip install google-generativeai python-dotenv
```

### 2. Create `.env` file:

```env
GEMINI_API_KEY=your_api_key_here
```

---

## 🚀 How to Run

### 1️⃣ Part 1: Paragraph Retrieval (DSA Only)

* **Purpose**: Find the most relevant paragraphs for a given question.
* Edit `tester.cpp`:

  * Set your question and desired top-k.
  * **Comment out** the line:

    ```cpp
    // qna_tool.query(question, "api_call.py");
    ```

#### Compile & Run:

```bash
g++ Node.cpp dict.cpp qna_tool.cpp tester.cpp -o f
./f
```

* 📤 Output:

  * Top-k paragraphs printed in terminal
  * Also saved to `top_k_paragraphs.txt`

---

### 2️⃣ Part 2: Answer with Gemini (LLM-Enhanced)

* **Purpose**: Get the final answer using Gemini with only top-k context.
* Edit `tester.cpp`:

  * Set the question
  * **Comment out Part 1 logic**
  * **Keep this line**:

    ```cpp
    qna_tool.query(question, "api_call.py");
    ```

#### Compile & Run:

```bash
g++ Node.cpp dict.cpp qna_tool.cpp tester.cpp -o f
./f
```

* 📤 Output:

  * Prints relevant paragraphs
  * Then prints Gemini’s answer


## 🧠 Concepts Used

* Tries for prefix matching
* Max-heaps for top-k selection
* Token-efficient LLM queries
* Clean separation of corpus querying and answer generation logic